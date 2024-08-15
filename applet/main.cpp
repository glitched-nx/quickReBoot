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
#include <payload.hpp>
#include <util.hpp>

#include <cstdio>
#include <cstdlib>
#include <switch.h>
#include <vector>

namespace {

    /**
     * @brief Typedef für eine Callback-Funktion, die einen konstanten Zeiger auf Benutzerdaten akzeptiert.
     */
    typedef void (*TuiCallback)(void const *const user);

    /**
     * @brief Struktur zur Darstellung eines TUI-Elements.
     * 
     * @param text Der anzuzeigende Text des Elements.
     * @param cb Die Callback-Funktion, die beim Auswählen des Elements aufgerufen wird.
     * @param user Ein konstanter Zeiger auf Benutzerdaten, die an die Callback-Funktion übergeben werden.
     * @param selectable Gibt an, ob das Element auswählbar ist.
     */
    struct TuiItem {
        std::string const text;
        TuiCallback const cb;
        void const *const user;
        bool const selectable;
    };

    /**
     * @brief Callback-Funktion zum Neustarten in eine Hekate-Konfiguration.
     * 
     * @param user Ein konstanter Zeiger auf die Hekate-Konfiguration.
     */
    void BootConfigCallback(void const *const user) {
        auto const config = reinterpret_cast<Payload::HekateConfig const *>(user);

        Payload::RebootToHekateConfig(*config, false);
    }

    /**
     * @brief Callback-Funktion zum Neustarten in eine Hekate-INI-Konfiguration.
     * 
     * @param user Ein konstanter Zeiger auf die Hekate-INI-Konfiguration.
     */
    void IniConfigCallback(void const *const user) {
        auto const config = reinterpret_cast<Payload::HekateConfig const *>(user);

        Payload::RebootToHekateConfig(*config, true);
    }

    /**
     * @brief Callback-Funktion zum Neustarten in den UMS-Modus (USB Mass Storage).
     * 
     * @param user Ein konstanter Zeiger auf Benutzerdaten (nicht verwendet).
     */
    void UmsCallback(void const *const user) {
        Payload::RebootToHekateUMS(Payload::UmsTarget_Sd);

        (void)user;
    }

    /**
     * @brief Callback-Funktion zum Neustarten in eine Payload-Konfiguration.
     * 
     * @param user Ein konstanter Zeiger auf die Payload-Konfiguration.
     */
    void PayloadCallback(void const *const user) {
        auto const config = reinterpret_cast<Payload::PayloadConfig const *>(user);

        Payload::RebootToPayload(*config);
    }
}

/**
 * @brief Initialisiert die Benutzeranwendung.
 * 
 * Initialisiert die SPSM-, SPL- und I2C-Dienste.
 */
extern "C" void userAppInit(void) {
    spsmInitialize();
    splInitialize();
    i2cInitialize();
}

/**
 * @brief Beendet die Benutzeranwendung.
 * 
 * Beendet die I2C-, SPL- und SPSM-Dienste.
 */
extern "C" void userAppExit(void) {
    i2cExit();
    splExit();
    spsmExit();
}

/**
 * @brief Hauptfunktion des Programms.
 * 
 * Diese Funktion initialisiert die Konsole und das Eingabesystem, lädt verfügbare Konfigurationen und Payloads,
 * erstellt ein Menü und verarbeitet Benutzereingaben, um verschiedene Neustartoptionen zu ermöglichen.
 * 
 * @param argc Anzahl der Argumente.
 * @param argv Array der Argumente.
 * @return int Rückgabewert des Programms (EXIT_SUCCESS bei Erfolg).
 */
int main(int const argc, char const *argv[]) {
    if (!util::IsErista() && !util::SupportsMarikoRebootToConfig()) {
        consoleInit(nullptr);

        printf("mind. Atmosphère 1.6.1\n");

        consoleUpdate(nullptr);

        /* Konfiguriere Eingabe */
        padConfigureInput(8, HidNpadStyleSet_NpadStandard);

        /* Initialisiere Pad */
        PadState pad;
        padInitializeAny(&pad);

        while (appletMainLoop()) {
            /* Aktualisiere Pad-Status */
            padUpdate(&pad);

            if (padGetButtonsDown(&pad))
                break;
        }

        consoleExit(nullptr);
        return 0;
    }

    std::vector<TuiItem> items;

    /* Lade verfügbare Boot-Konfigurationen */
    auto const boot_config_list = Payload::LoadHekateConfigList();

    /* Lade verfügbare INI-Konfigurationen */
    auto const ini_config_list = Payload::LoadIniConfigList();

    /* Lade verfügbare Payloads */
    auto const payload_config_list = Payload::LoadPayloadList();

    /* Erstelle Menüelementliste */
    items.reserve(2 + boot_config_list.empty() ? 0 : 1 + boot_config_list.size()
                    + ini_config_list.empty()  ? 0 : 1 + ini_config_list.size()
                    + payload_config_list.empty()  ? 0 : 1 + payload_config_list.size());

    if (!boot_config_list.empty()) {
        items.emplace_back("reLoads to...", nullptr, nullptr, false);
        for (auto const &entry : boot_config_list)
            items.emplace_back(entry.name, BootConfigCallback, &entry, true);
    }

    if (!ini_config_list.empty()) {
        items.emplace_back("Hekate - More Configs", nullptr, nullptr, false);
        for (auto const &entry : ini_config_list)
            items.emplace_back(entry.name, IniConfigCallback, &entry, true);
    }

    items.emplace_back("Various ReLoads", nullptr, nullptr, false);
    items.emplace_back("reLoad to UMS", UmsCallback, nullptr, true);

    if (util::IsErista() && !payload_config_list.empty()) {
        items.emplace_back("payloads", nullptr, nullptr, false);
        for (auto const &entry : payload_config_list)
            items.emplace_back(entry.name, PayloadCallback, &entry, true);
    }

    std::size_t index = 0;

    for (auto const &item : items) {
        if (item.selectable)
            break;

        index++;
    }

    PrintConsole *const console = consoleInit(nullptr);

    /* Konfiguriere Eingabe */
    padConfigureInput(8, HidNpadStyleSet_NpadStandard);

    /* Initialisiere Pad */
    PadState pad;
    padInitializeAny(&pad);

    /* Deinitialisiere sm, um unseren einzigen Dienstslot freizugeben */
    smExit();

    bool repaint = true;

    while (appletMainLoop()) {
        {
            /* Aktualisiere Pad-Status */
            padUpdate(&pad);

            u64 const kDown = padGetButtonsDown(&pad);

            if ((kDown & (HidNpadButton_Plus | HidNpadButton_B | HidNpadButton_L)))
                break;

            if ((kDown & HidNpadButton_A)) {
                auto &item = items[index];

                if (item.selectable && item.cb)
                    item.cb(item.user);
            }

            if ((kDown & HidNpadButton_Minus)) {
                Payload::RebootToHekate();
            }

            if ((kDown & HidNpadButton_AnyDown) && (index + 1) < items.size()) {
                for (std::size_t i = index; i < items.size(); i++) {
                    if (!items[i + 1].selectable)
                        continue;

                    index = i + 1;
                    break;
                }
                repaint = true;
            }

            if ((kDown & HidNpadButton_AnyUp) && index > 0) {
                for (std::size_t i = index; i > 0; i--) {
                    if (!items[i - 1].selectable)
                        continue;

                    index = i - 1;
                    break;
                }
                repaint = true;
            }
        }

        if (repaint) {
            consoleClear();

            std::printf("quickReLoader\n-------------\n");

            for (std::size_t i = 0; i < items.size(); i++) {
                auto const &item    = items[i];
                bool const selected = (i == index);

                if (!item.selectable)
                    console->flags |= CONSOLE_COLOR_FAINT;

                std::printf("%c %s\n", selected ? '>' : ' ', item.text.c_str());

                if (!item.selectable)
                    console->flags &= ~CONSOLE_COLOR_FAINT;
            }
            repaint = false;
        }

        consoleUpdate(nullptr);
    }

    consoleExit(nullptr);

    (void)argc;
    (void)argv;

    return EXIT_SUCCESS;
}
