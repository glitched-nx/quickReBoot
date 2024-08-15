#define TESLA_INIT_IMPL // Wenn mehr als eine Datei den Tesla-Header verwendet, sollte dies nur in der Hauptdatei definiert werden.
#include <tesla.hpp>    // Der Tesla-Header


/**
 * @class GuiSecondary
 * @brief Eine Klasse, die eine sekundäre GUI für das Tesla-Overlay darstellt.
 */
class GuiSecondary : public tsl::Gui {
public:
    /**
     * @brief Konstruktor für die GuiSecondary-Klasse.
     */
    GuiSecondary() {}

    /**
     * @brief Erstellt die Benutzeroberfläche für diese GUI.
     * 
     * @return Ein Zeiger auf das erstellte Element, das die Benutzeroberfläche darstellt.
     */
    virtual tsl::elm::Element* createUI() override {
        // Erstellen eines OverlayFrame mit Titel und Untertitel
        auto *rootFrame = new tsl::elm::OverlayFrame("Tesla Example", "v1.3.2 - Secondary Gui");

        // Setzen des Inhalts des Frames auf ein Debug-Rechteck
        rootFrame->setContent(new tsl::elm::DebugRectangle(tsl::Color{ 0x8, 0x3, 0x8, 0xF }));

        return rootFrame;
    }
};

/**
 * @class GuiTest
 * @brief Eine Klasse, die die Haupt-GUI für das Tesla-Overlay darstellt.
 */
class GuiTest : public tsl::Gui {
public:
    /**
     * @brief Konstruktor für die GuiTest-Klasse.
     * 
     * @param arg1 Ein Argument vom Typ u8.
     * @param arg2 Ein weiteres Argument vom Typ u8.
     * @param arg3 Ein Argument vom Typ bool.
     */
    GuiTest(u8 arg1, u8 arg2, bool arg3) { }

    /**
     * @brief Erstellt die Benutzeroberfläche für diese GUI.
     * 
     * @return Ein Zeiger auf das erstellte Element, das die Benutzeroberfläche darstellt.
     */
    virtual tsl::elm::Element* createUI() override {
        // Ein OverlayFrame ist das Basiselement, aus dem jedes Overlay besteht. Dies wird den Standardtitel und Untertitel zeichnen.
        // Wenn mehr Informationen in der Kopfzeile benötigt werden oder deren Aussehen geändert werden soll, verwenden Sie ein HeaderOverlayFrame.
        auto frame = new tsl::elm::OverlayFrame("Tesla Example", "v1.3.2");

        // Eine Liste, die Unterelemente enthalten kann und das Scrollen verwaltet
        auto list = new tsl::elm::List();

        // Listenelemente
        list->addItem(new tsl::elm::CategoryHeader("List items"));

        auto *clickableListItem = new tsl::elm::ListItem("Clickable List Item", "...");
        clickableListItem->setClickListener([](u64 keys) {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiSecondary>();
                return true;
            }

            return false;
        });

        // Füge das klickbare Listenelement zur Liste hinzu
        list->addItem(clickableListItem);

        // Füge ein Standard-Listenelement zur Liste hinzu
        list->addItem(new tsl::elm::ListItem("Default List Item"));

        // Füge ein Standard-Listenelement mit einem extra langen Namen hinzu, um die Trunkierung und das Scrollen zu testen
        list->addItem(new tsl::elm::ListItem("Default List Item with an extra long name to trigger truncation and scrolling"));

        // Füge ein umschaltbares Listenelement zur Liste hinzu
        list->addItem(new tsl::elm::ToggleListItem("Toggle List Item", true));

        // Custom Drawer, ein Element, das direkten Zugriff auf den Renderer gibt
        list->addItem(new tsl::elm::CategoryHeader("Custom Drawer", true));
        list->addItem(new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawCircle(x + 40, y + 40, 20, true, renderer->a(0xF00F));
            renderer->drawCircle(x + 50, y + 50, 20, true, renderer->a(0xF0F0));
            renderer->drawRect(x + 130, y + 30, 60, 40, renderer->a(0xFF00));
            renderer->drawString("Hello :)", false, x + 250, y + 70, 20, renderer->a(0xFF0F));
            renderer->drawRect(x + 40, y + 90, 300, 10, renderer->a(0xF0FF));
        }), 100);

        // Füge Trackbars zur Liste hinzu
        list->addItem(new tsl::elm::CategoryHeader("Track bars"));
        list->addItem(new tsl::elm::TrackBar("\u2600"));
        list->addItem(new tsl::elm::StepTrackBar("\uE13C", 20));
        list->addItem(new tsl::elm::NamedStepTrackBar("\uE132", { "Selection 1", "Selection 2", "Selection 3" }));

        // Füge die Liste zum Frame hinzu, damit sie gezeichnet wird
        frame->setContent(list);

        // Gib den Frame zurück, damit er das oberste Element dieser GUI wird
        return frame;
    }

    /**
     * @brief Wird einmal pro Frame aufgerufen, um Werte zu aktualisieren.
     */
    virtual void update() override {

    }

    /**
     * @brief Wird einmal pro Frame aufgerufen, um Eingaben zu verarbeiten, die nicht von anderen UI-Elementen verarbeitet wurden.
     * 
     * @param keysDown Die Tasten, die in diesem Frame gedrückt wurden.
     * @param keysHeld Die Tasten, die in diesem Frame gehalten wurden.
     * @param touchPos Die Position des Touch-Eingangs.
     * @param joyStickPosLeft Die Position des linken Analogsticks.
     * @param joyStickPosRight Die Position des rechten Analogsticks.
     * @return true, wenn die Eingaben verarbeitet wurden, andernfalls false.
     */
    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        return false;   // Return true here to signal the inputs have been consumed
    }
};

class OverlayTest : public tsl::Overlay {
public:
    /**
     * @brief Wird zu Beginn aufgerufen, um alle für dieses Overlay notwendigen Dienste zu initialisieren.
     */
    virtual void initServices() override {}  

    /**
     * @brief Wird am Ende aufgerufen, um alle zuvor initialisierten Dienste zu bereinigen.
     */
    virtual void exitServices() override {}  

    /**
     * @brief Wird aufgerufen, bevor das Overlay von unsichtbar zu sichtbar wechseln möchte.
     */
    virtual void onShow() override {}    

    /**
     * @brief Wird aufgerufen, bevor das Overlay von sichtbar zu unsichtbar wechseln möchte.
     */
    virtual void onHide() override {}    

    /**
     * @brief Lädt die initiale GUI. Es ist möglich, Argumente an den Konstruktor zu übergeben.
     * 
     * @return Ein einzigartiger Zeiger auf die initiale GUI.
     */
    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiTest>(1, 2, true);  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

/**
 * @brief Der Haupteinstiegspunkt der Anwendung.
 * 
 * @param argc Die Anzahl der Argumente.
 * @param argv Das Array der Argumente.
 * @return Der Rückgabewert der Anwendung.
 */
int main(int argc, char **argv) {
    return tsl::loop<OverlayTest>(argc, argv);
}
