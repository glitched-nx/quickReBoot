/*
 * Copyright (c) 2020-2023 Studious Pancake
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define TESLA_INIT_IMPL
#include <payload.hpp>
#include <util.hpp>

#include <tesla.hpp>

namespace {

    constexpr const char AppTitle[] = APP_TITLE;
    constexpr const char AppVersion[] = APP_VERSION;

}

/**
 * @brief Klasse für die GUI des Pancake-Overlays.
 */
class PancakeGui : public tsl::Gui {
  private:
    Payload::HekateConfigList const boot_config_list; ///< Liste der Boot-Konfigurationen.
    Payload::HekateConfigList const ini_config_list; ///< Liste der INI-Konfigurationen.
    Payload::PayloadConfigList const payload_config_list; ///< Liste der Payload-Konfigurationen.

  public:
    /**
     * @brief Konstruktor, lädt die Konfigurationslisten.
     */
    PancakeGui()
        : boot_config_list(Payload::LoadHekateConfigList()),
          ini_config_list(Payload::LoadIniConfigList()),
          payload_config_list(Payload::LoadPayloadList()) {
    }

    /**
     * @brief Erstellt die Benutzeroberfläche.
     * @return Zeiger auf das erstellte UI-Element.
     */
    virtual tsl::elm::Element *createUI() override {
        auto const frame = new tsl::elm::OverlayFrame(AppTitle, AppVersion);

        auto const list = new tsl::elm::List();

        /* Boot-Konfigurationseinträge hinzufügen. */
        if (!boot_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("quickReLoad to..."));

            for (auto const &config : boot_config_list) {
                auto const entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToHekateConfig(config, false); });
                list->addItem(entry);
            }
        }

        /* INI-Konfigurationseinträge hinzufügen. */
        if (!ini_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("more reLoads"));

            for (auto const &config : ini_config_list) {
                auto const entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToHekateConfig(config, true); });
                list->addItem(entry);
            }
        }

        /* Verschiedenes. */
        list->addItem(new tsl::elm::CategoryHeader("other Stuff"));

        auto const ums = new tsl::elm::ListItem("SD <-UMS-> PC");
        ums->setClickListener([](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToHekateUMS(Payload::UmsTarget_Sd); });
        list->addItem(ums);

        /* Payloads */
        if (util::IsErista() && !payload_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("payloads"));

            for (auto const &config : payload_config_list) {
                auto const entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToPayload(config); });
                list->addItem(entry);
            }
        }

        frame->setContent(list);
        return frame;
    }

    /**
     * @brief Behandelt Benutzereingaben.
     * @param keysDown Gedrückte Tasten.
     * @param keysHeld Gehaltene Tasten.
     * @param touchPos Touch-Position.
     * @param joyStickPosLeft Position des linken Analogsticks.
     * @param joyStickPosRight Position des rechten Analogsticks.
     * @return true, wenn die Eingabe behandelt wurde, sonst false.
     */
    virtual bool handleInput(u64 const keysDown, u64 const keysHeld, const HidTouchState &touchPos, HidAnalogStickState const joyStickPosLeft, HidAnalogStickState const joyStickPosRight) {
        if (keysDown & HidNpadButton_Minus)
            Payload::RebootToHekate();

        (void)keysHeld;
        (void)touchPos;
        (void)joyStickPosLeft;
        (void)joyStickPosRight;

        return false;
    }
};

/**
 * @brief GUI-Klasse, die eine Aktualisierung anfordert.
 */
class PleaseUpdateGui final : public tsl::Gui {
    /**
     * @brief Erstellt die Benutzeroberfläche.
     * @return Zeiger auf das erstellte UI-Element.
     */
    virtual tsl::elm::Element *createUI() override {
        auto const frame = new tsl::elm::OverlayFrame(AppTitle, AppVersion);

        auto const custom = new tsl::elm::CustomDrawer([msgW = 0, msgH = 0](tsl::gfx::Renderer *drawer, u16 x, u16 y, u16 w, u16 h) mutable {
            drawer->drawString("\uE150", false, x + (w / 2) - (90 / 2), 300, 90, 0xffff);
            auto [width, height] = drawer->drawString("min. Atmosphère 1.6.1\n", false, x + (w / 2) - (msgW / 2), 380, 25, 0xffff);
            if (msgW == 0) {
                msgW = width;
                msgH = height;
            }
        });

        frame->setContent(custom);

        return frame;
    }
};

/**
 * @brief Overlay-Klasse für das Pancake-Overlay.
 */
class PancakeOverlay final : public tsl::Overlay {
  public:
    /**
     * @brief Initialisiert die benötigten Dienste.
     */
    virtual void initServices() override {
        fsdevMountSdmc();
        splInitialize();
        spsmInitialize();
        i2cInitialize();
    }

    /**
     * @brief Beendet die initialisierten Dienste.
     */
    virtual void exitServices() override {
        i2cExit();
        spsmExit();
        splExit();
        fsdevUnmountAll();
    }

    /**
     * @brief Lädt die initiale GUI.
     * @return Einzigartiger Zeiger auf das initiale GUI-Element.
     */
    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        if (!util::IsErista() && !util::SupportsMarikoRebootToConfig()) {
            return std::make_unique<PleaseUpdateGui>();
        } else {
            return std::make_unique<PancakeGui>();
        }
    }
};

/**
 * @brief Haupteinstiegspunkt des Programms.
 * @param argc Anzahl der Argumente.
 * @param argv Argumentvektor.
 * @return Programmstatuscode.
 */
int main(int argc, char **argv) {
    return tsl::loop<PancakeOverlay>(argc, argv);
}