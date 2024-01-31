// Prism Menu by Firee
// i am looking at you. make sure you credit me and this mod, i worked hard on it ;-;
// please do not judge my coding, ok thank you!!!
#include "CustomSettings.hpp"
#include <Geode/utils/file.hpp>
#include "hacks.hpp"
#include "Languages.hpp"
#include "Themes.hpp"
#include "PrismUI.hpp"
#include "Utils.hpp"
#include <Geode/Geode.hpp>
//#include <geode.custom-keybinds/include/Keybinds.hpp>
//using namespace keybinds;

#include <iomanip>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>

using namespace geode::prelude;

// ImGUI
bool showMenu = false;
int currentMenuIndex = 0;

struct EditMode {
    std::string name = "";
    HackValue editedValue = 0.F;
    bool isEditing = false;
};

EditMode editingMode;

#ifndef GEODE_IS_MACOS
#include <imgui-cocos.hpp>
#endif
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/AchievementNotifier.hpp>



std::string determineName(const char* name, float width) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter; // what is this nonsense chat jippity, i just wanna handle unicode!

    std::string nameStr(name);
    if (width <= 125) {
        if (width <= 80) {
            std::wstring wideName = converter.from_bytes(name);
            return converter.to_bytes(wideName.substr(0, 1));
        } else {
            return nameStr.substr(2);
        }
    } else {
        return nameStr;
    }
}
#ifndef GEODE_IS_MACOS
struct ItemRectInfo {
    ImVec2 min;
    ImVec2 max;
};

ItemRectInfo g_ItemRectInfo;

void RoundTableBG() {
    auto currentTheme = Themes::getCurrentTheme();
    auto color = Themes::RGBAToImU32(currentTheme["TableRowBg"]);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 min = g_ItemRectInfo.min;
    ImVec2 max = g_ItemRectInfo.max;
    drawList->AddRectFilled(min, max, color, 15.0F, ImDrawFlags_RoundCornersAll);
}

// easier
void CreateButton(const char* name, int menuIndex, float buttonWidth, float buttonHeight) {
    if (currentMenuIndex == menuIndex) {
        auto currentTheme = Themes::getCurrentTheme();
        ImGui::BeginDisabled();
        //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, Themes::RGBAToImVec4(currentTheme["ButtonActive"]));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0F, 1.0F, 1.0F, 1.0F));
    }
    if (ImGui::Button(determineName(name, buttonWidth).c_str(), { buttonWidth, buttonHeight })) {
        currentMenuIndex = menuIndex;
    };
    if (currentMenuIndex == menuIndex) {
        ImGui::EndDisabled();
        ImGui::PopStyleColor(2);
    }
    //ImGui::PushStyleColor();
}
#endif

/*
for i in range(char2nr('A'), char2nr('Z'))
  call append(line("."), printf("{'%c', cocos2d::KEY_%c},", i, i))
endfor
*/

class PrismButton : public CCMenu {
protected:
    CCTouch* touchs;
    //CCMenuItemSpriteExtra* menuButton;
    bool allowDragging;
    bool isTouchOnButton;
    float initialTouchX;
    float initialTouchY;
    float touchHoldTimer;

    bool init(CCScene* p0) {
        if (!CCMenu::init())
            return false;
        HackItem* posX = Hacks::getHack("Button Position X");
        HackItem* posY = Hacks::getHack("Button Position Y");
        auto menuButton = createButton(this);
        this->addChild(menuButton);
        this->setPositionX(posX->value.intValue);
        this->setPositionY(posY->value.intValue);
        this->registerWithTouchDispatcher();
        this->setTouchEnabled(true);
        this->setZOrder(p0->getHighestChildZ() + 100);
        allowDragging = true;
        isTouchOnButton = false;
        touchHoldTimer = 0.0f;
        //this->scheduleUpdate();
        return true;
    }
    void registerWithTouchDispatcher() override { // is this even needed anymore
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
    }
public:
    // chat jippity 
#if 0 // fixing later, because chatgpt is givin gme bad results
    void update(float dt) {
        updateTouchHoldTimer(dt);
        // Check if the touch is on the button and has been held for 0.5 seconds
        if (isTouchOnButton && allowDragging && touchHoldTimer >= 0.5) {
            std::cout << "do you" << std::endl;
            // Calculate new position based on touch movement
            float deltaX = touchs->getDelta().x;
            float deltaY = touchs->getDelta().y;
            this->setPositionX(this->getPositionX() + deltaX);
            this->setPositionY(this->getPositionY() + deltaY);

            // Set opacity to 50%
            this->setOpacity(128); // 50% opacity

        } else {
            // If not dragging, set opacity back to 100%
            this->setOpacity(255); // 100% opacity
        }
    }


    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        this->touchs = touch;
        std::cout << "ccTouchBegan" << std::endl;
        CCRect rect = menuButton->boundingBox();
        if (rect.containsPoint(touch->getLocation())) {
            std::cout << "ccTouchBegan you are TRUE!" << std::endl;
            isTouchOnButton = true;
            initialTouchX = touch->getLocation().x;
            initialTouchY = touch->getLocation().y;
            allowDragging = false; // Reset flag
            touchHoldTimer = 0.0F;
            return true;
        }

        return false;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        // Check if the touch is on the button
        if (isTouchOnButton) {
            float distance = ccpDistance(ccp(initialTouchX, initialTouchY), touch->getLocation());
            std::cout << "ccTouchMoved " << distance << std::endl;
            if (distance > 10) {
                std::cout << "ccTouchBegan ya" << std::endl;
                allowDragging = true; // Enable dragging if the touch has moved by more than 10 pixels
            }
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        // Check if the touch is on the button
        if (isTouchOnButton) {
            std::cout << "ccTouchEnded" << std::endl;
            // If not dragging, call onButtonClicked
            if (!allowDragging) {
                onButtonClicked(nullptr);
            }

            // Reset flags and properties
            isTouchOnButton = false;
            allowDragging = false;
            touchHoldTimer = 0.0F;
        }
    }

    void updateTouchHoldTimer(float dt) {
        if (isTouchOnButton) {
            std::cout << "updateTouchHoldTimer " << touchHoldTimer << std::endl;
            touchHoldTimer += dt;
        }
    }
#endif
    static CCMenuItemSpriteExtra* createButton(CCLayer* ret) {
        auto myButtonSpr = CircleButtonSprite::create(CCSprite::create("icon.png"_spr));
        return CCMenuItemSpriteExtra::create(myButtonSpr, ret, menu_selector(PrismButton::onButtonClicked));
    }
    void onButtonClicked(CCObject* p0) {
        HackItem* menuStyle = Hacks::getHack("Menu Style");
        if (menuStyle->value.intValue == 0) { // imgui
            showMenu = !showMenu;
        } else {
            PrismUI::create()->show();
        }
    }

    static PrismButton* create(CCScene* p0) {
        auto ret = new PrismButton();
        if (ret && ret->init(p0)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

PrismButton* prismButton;


bool firstLoad = false;
bool changedOpacity = false;
bool changedMenuScale = false;
// early load is amazing!

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        /*auto username = static_cast<CCLabelBMFont*>(this->getChildByID("player-username"));
        if (username != nullptr) {
            auto normal = CCSprite::createWithSpriteFrameName("difficulty_02_btn_001.png");
            normal->setPosition({44,91});
            normal->setScale(2.0F);
            this->addChild(normal);
            username->setString("FIRE IN THE HOLE");
        }*/
        // lmao
        HackItem* posX = Hacks::getHack("Button Position X");
        HackItem* posY = Hacks::getHack("Button Position Y");
        auto mainMenu = static_cast<CCMenu*>(this->getChildByID("bottom-menu"));
        if (firstLoad) return true;
        firstLoad = true;
        log::info("Prism Menu loaded! Enjoy the mod.");
        // TODO: goodbye imgui
        #ifndef GEODE_IS_MACOS
        ImGuiCocos::get().setup([] {
            /*
            // Future Dark style by rewrking from ImThemes
            ImGuiStyle& style = ImGui::GetStyle();
            ImGuiIO& io = ImGui::GetIO();
            ImFontConfig config;
            config.GlyphRanges = io.Fonts->GetGlyphRangesCyrillic();
            std::string fontName = (Mod::get()->getResourcesDir() / "PrismMenu.otf").string();
            //std::string fontName = (Mod::get()->getResourcesDir() / "Hack-Regular.ttf").string();
            io.Fonts->AddFontFromFileTTF(fontName.c_str(), 24, &config);
            io.Fonts->AddFontFromFileTTF(fontName.c_str(), 30, &config);
            // TODO, look at gamershackers or idk his cchannels tutorials on adding images to add icons
            
            style.Alpha = 1.0f;
            style.DisabledAlpha = 1.0f;
            style.WindowPadding = ImVec2(12.0f, 12.0f);
            style.WindowRounding = 15.0f;
            style.WindowBorderSize = 0.0f;
            style.WindowMinSize = ImVec2(20.0f, 20.0f);
            style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
            style.WindowMenuButtonPosition = ImGuiDir_None;
            
            style.ChildRounding = 0.0f;
            style.ChildBorderSize = 1.0f;
            
            style.PopupRounding = 0.0f;
            style.PopupBorderSize = 1.0f;
            
            style.FramePadding = ImVec2(6.0f, 6.0f);
            style.FrameRounding = 0.0f;
            style.FrameBorderSize = 0.0f;
            
            style.ItemSpacing = ImVec2(12.0f, 6.0f);
            style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
            
            style.CellPadding = ImVec2(12.0f, 6.0f);
            
            style.IndentSpacing = 20.0f;
            style.ColumnsMinSpacing = 6.0f;
            style.ScrollbarSize = 12.0f;
            style.ScrollbarRounding = 20.0f;
            style.GrabMinSize = 12.0f;
            style.GrabRounding = 0.0f;
            style.TabRounding = 0.0f;
            style.TabBorderSize = 0.0f;
            style.TabMinWidthForCloseButton = 0.0f;
            style.ColorButtonPosition = ImGuiDir_Right;
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
            
            float currentOpacity = Hacks::getHack("Menu Opacity")->value.floatValue;
            style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, currentOpacity);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, currentOpacity);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, currentOpacity);
            style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5529412031173706f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
            style.Colors[ImGuiCol_Separator] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
            style.Colors[ImGuiCol_Tab] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
            style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
            style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
            style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
            style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
            style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            ///style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
            ///style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 0.0f);
            style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            //style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
            //style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
            style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 0.5f);
            style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 0.5f);
            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
            style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
            style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
            //style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);*/ 
            Themes::LoadTheme(Themes::getCurrentTheme());
        }).draw([] {
            if (showMenu) {
                // i cant use LoadTheme because 0160:err:virtual:allocate_virtual_memory out of memory for allocation, base (nil) size
                // well just in case 
                if (Hacks::isHackEnabled("Live Theme Editing") || changedMenuScale) {
                    Themes::LoadTheme(Themes::getCurrentTheme());
                    changedMenuScale = false;
                } else {
                    if (changedOpacity) {
                        Themes::UpdateOpacity(Themes::getCurrentTheme());
                        changedOpacity = false;
                    }
                }

                auto* director = CCDirector::sharedDirector();
                const auto winSize = director->getWinSize();
                const auto frameSize = director->getOpenGLView()->getFrameSize();
                //std::cout << fmt::format("{},{} and {},{}",winSize.width,winSize.height,frameSize.width,frameSize.height) << std::endl;
                // 570, 320
                //ImGui::SetNextWindowSize({ 1000, 480 }/*, ImGuiCond_FirstUseEver*/);
                //ImGui::SetNextWindowSize({ 1.8F * winSize.width, 1.5F * winSize.height });
                float width = (1000 * frameSize.width) / 1366;
                float height = (500 * frameSize.height) / 768;
                float tableWidth = (220 * frameSize.width) / 1366;

                auto currentLanguage = Lang::getLanguage();
                ImGui::SetNextWindowSize({ width, height });
                ImGui::Begin("Prism Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
                //231,245 - 451,701
                RoundTableBG();
                if (ImGui::BeginTable("table_left", 1, ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV, { tableWidth, 0 })) // ImGuiTableFlags_RowBg | ImGuiTableFlags_None |
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None, 0);
                    //ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None, 4);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    // Categories
                    ImFont* oldFont = ImGui::GetFont(); // Save the current font
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Change to a different font in the font atlas

                    ImGui::Spacing();
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 13.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
                    // here we do a genius trick, please dont ask me how i did this! ok! this took forever to do
                    // chat jippity gave icon ideas, i designed the player!
                    // oh and PLEASE dont complain about it being misaligned in the center
                    float buttonWidth = (194 * frameSize.width) / 1366;
                    float buttonHeight = (60 * frameSize.height) / 768;
                    CreateButton(currentLanguage->name("§ Global").c_str(), 0, buttonWidth, buttonHeight);
                    CreateButton(currentLanguage->name("¬ Player").c_str(), 1, buttonWidth, buttonHeight);
                    CreateButton(currentLanguage->name("ª Bypass").c_str(), 2, buttonWidth, buttonHeight);
                    CreateButton(currentLanguage->name("« Creator").c_str(), 3, buttonWidth, buttonHeight);
                    CreateButton(currentLanguage->name("··· Misc").c_str(), 4, buttonWidth, buttonHeight);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::PopStyleVar(2);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
                    // ad "UI Labels"
                    CreateButton(currentLanguage->name("¶ Settings").c_str(), 5, buttonWidth, buttonHeight);
                    ImGui::PopStyleVar();
                    ImGui::PopFont(); // Restore the old font
                    //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 15.0f));
                    ImGui::Spacing();
                    ImGui::EndTable();
                }
                g_ItemRectInfo.min = ImGui::GetItemRectMin();
                g_ItemRectInfo.max = ImGui::GetItemRectMax();
                ImGui::SameLine();
                if (ImGui::BeginTable("table1_right", 1, ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) { //ImGuiTableFlags_ScrollX | 
                    // Buttons
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None, 0);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    std::vector<matjson::Value> jsonArray;
                    switch (currentMenuIndex) {
                        case 0: // Global 
                            jsonArray = matjson::parse(Hacks::getGlobalHacks()).as_array();
                            break;
                        case 1: // Player
                            jsonArray = matjson::parse(Hacks::getPlayerHacks()).as_array();
                            break;
                        case 2: // Bypass
                            jsonArray = matjson::parse(Hacks::getBypassHacks()).as_array();
                            break;
                        case 3: // Creator
                            jsonArray = matjson::parse(Hacks::getCreatorHacks()).as_array();
                            break;
                        case 4: // Misc
                            jsonArray = matjson::parse(Hacks::getMiscHacks()).as_array();
                            break;
                        case 5: // Settings
                            jsonArray = matjson::parse(Hacks::getSettings()).as_array();
                            ImGui::Text("%s", currentLanguage->name("Prism Menu by Firee").c_str());
                            //const char* version = "V1.3.0 (Geode)";
                            #ifdef GITHUB_ACTIONS
                            auto version = fmt::format("{} (Geode)", Mod::get()->getVersion().toString());
                            #else 
                            auto version = fmt::format("{}-Dev (Geode)", Mod::get()->getVersion().toString());
                            #endif
                           #ifdef GEODE_IS_WINDOWS
                            ImGui::Text("%s - Windows", version.c_str());
                            #else // why does android not like elif
                            #ifdef GEODE_IS_ANDROID
                                ImGui::Text("%s - Android", version.c_str());
                            #else 
                                ImGui::Text("%s - HOW by Spu7nix", version.c_str());
                            #endif
                            #endif
                            ImGui::Separator();
                            break;
                    }
                    ImGui::Spacing();
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
                    auto settings = Mod::get()->getSavedValue<SettingHackStruct>("values");
                    for (size_t i = 0; i < jsonArray.size(); i++) {
                        const auto& obj = jsonArray[i];
                        std::string name = obj.get<std::string>("name");
                        std::string desc = obj.get<std::string>("desc");
                        auto opcodes = obj.get<matjson::Array>("opcodes");
                        HackItem* hack = Hacks::getHack(name);
                        if (hack != nullptr) {
                            if (obj.contains("winOnly")) {
                                //#ifndef GEODE_IS_WINDOWS
                                ImGui::BeginDisabled(true);
                                //#endif
                            }
                            if (obj.contains("win")) { // yeah idk what property okay
                                #ifndef GEODE_IS_WINDOWS
                                ImGui::BeginDisabled(true);
                                #endif
                            }
                            if (hack->value.type == ValueType::Int && hack->type != "dropdown") {
                                auto min = obj.get<int>("min");
                                auto max = obj.get<int>("max");
                                int step = (obj.contains("step")) ? obj.get<int>("step") : 5;
                                int oldValue = hack->value.intValue;
                                if (ImGui::InputInt(currentLanguage->name(name).c_str(), &hack->value.intValue, step, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
                                    if (min > hack->value.intValue || hack->value.intValue > max) {
                                        hack->value.intValue = oldValue;
                                        return;
                                    }
                                    if (name == "FPS Bypass") {
                                        // from mats fps unlocker
                                        //Hacks::Settings::setSettingValue(&settings, *hack, hack->value.floatValue);
                                        auto app = CCApplication::sharedApplication();
                                        app->setAnimationInterval(1.0 / static_cast<double>(hack->value.intValue));
                                    } else if (name == "Button Position X") {
                                        Hacks::Settings::setSettingValue(&settings, *hack, hack->value.intValue);
                                        prismButton->setPositionX(hack->value.intValue);
                                    } else if (name == "Button Position Y") {
                                        Hacks::Settings::setSettingValue(&settings, *hack, hack->value.intValue);
                                        prismButton->setPositionY(hack->value.intValue);
                                    } else if (name == "Menu Style") {
                                        Hacks::Settings::setSettingValue(&settings, *hack, hack->value.intValue);
                                        showMenu = !showMenu;
                                    } else {
                                        Hacks::Settings::setSettingValue(&settings, *hack, hack->value.intValue);
                                    }
                                }
                            } else if (hack->value.type == ValueType::Float) {
                                auto min = obj.get<float>("min");
                                auto max = obj.get<float>("max");
                                if (!editingMode.isEditing || editingMode.name != name.c_str()) {
                                    if (ImGui::SliderFloat(currentLanguage->name(name).c_str(), &hack->value.floatValue, min, max, "%.3f", ImGuiSliderFlags_NoInput)) {
                                        if (name == "Speedhack") {
                                            if (hack->value.floatValue < 0.0F) return;
                                            Hacks::setPitch(hack->value.floatValue);
                                            CCDirector::sharedDirector()->getScheduler()->setTimeScale(hack->value.floatValue);
                                        } else {
                                            if (name == "Menu Opacity") {
                                                changedOpacity = true;
                                            } else if (name == "Menu Scale") {
                                                changedMenuScale = true;
                                            }
                                            Hacks::Settings::setSettingValue(&settings, *hack, hack->value.floatValue);
                                        }
                                    }
                                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                                        editingMode.isEditing = true;
                                        editingMode.name = hack->name;
                                        editingMode.editedValue.floatValue = hack->value.floatValue;
                                    }
                                } else {
                                    ImGui::BeginDisabled();
                                    ImGui::SliderFloat(currentLanguage->name(name).c_str(), &editingMode.editedValue.floatValue, min, max);
                                    ImGui::EndDisabled();
                                    ImGui::SetKeyboardFocusHere();
                                    if (ImGui::InputFloat("##EditValue", &editingMode.editedValue.floatValue, 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                                        hack->value.floatValue = editingMode.editedValue.floatValue;
                                        editingMode.isEditing = false;
                                        editingMode.name = "";
                                        Hacks::Settings::setSettingValue(&settings, *hack, hack->value.floatValue);
                                        if (name == "Speedhack") {
                                            if (hack->value.floatValue < 0.0F) {
                                                hack->value.floatValue = 1.0F;
                                            }
                                            Hacks::setPitch(hack->value.floatValue);
                                            CCDirector::sharedDirector()->getScheduler()->setTimeScale(hack->value.floatValue);
                                        } else if (name == "Menu Opacity") {
                                            changedOpacity = true;
                                        } else if (name == "Menu Scale") {
                                            changedMenuScale = true;
                                        }
                                    }
                                    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
                                        editingMode.isEditing = false;
                                        editingMode.name = "";
                                    }
                                }
                            } else if (hack->value.type == ValueType::Bool) {
                                if ((Hacks::isHackEnabled("Enable Patching") && obj.contains("winOnly")) || !obj.contains("winOnly") || name == "Enable Patching") {
                                    if (ImGui::Checkbox(currentLanguage->name(name).c_str(), &hack->value.boolValue)) {
                                        Hacks::Settings::setSettingValue(&settings, *hack, hack->value.boolValue);
                                        if (name == "Show Button") {
                                            prismButton->setVisible(hack->value.boolValue);
                                            // TODO: add a check to see if currently on MenuLayer
                                        }
                                        if (name == "Instant Complete" && hack->value.boolValue) {
                                            FLAlertLayer::create(nullptr, "Cheater!", "Just a warning, you will be <cr>banned off leaderboards</c> if you use this on rated levels. Consider this your <cy>warning</c>.", "OK", nullptr)->show();
                                        }
                                        if (Hacks::isHackEnabled("Enable Patching") && !opcodes.empty()) {
                                            #ifdef GEODE_IS_WINDOWS
                                            Hacks::applyPatches(name, opcodes, hack->value.boolValue);
                                            #endif
                                        }
                                    }
                                }
                            } else if (hack->value.type == ValueType::Char) {
                                /*auto oldValue = hack->value.charValue;
                                if (ImGui::InputTextWithHint(currentLanguage->name(name).c_str(), "C", hack->value.charValue, 2, ImGuiInputTextFlags_EnterReturnsTrue)) {
                                    if (name == "Open Menu Keybind") {
                                        auto it = charToKeyMap.find(*hack->value.charValue);
                                        if (it != charToKeyMap.end()) {
                                            std::cout << "Save" << std::endl;
                                            auto inputKeybind = it->second;
                                            BindManager::get()->removeBindable("prism-menu"_spr);
                                            BindManager::get()->registerBindable({
                                                "prism-menu"_spr,
                                                "Toggle Prism Menu",
                                                "Keybind for toggling when to show Prism Menu's Mod Menu.",
                                                { Keybind::create(KEY_A, Modifier::None) },
                                                "Prism Menu/ModMenu"
                                            });
                                            BindManager::get()->save();
                                        } else {
                                            hack->value.charValue = oldValue;
                                        }
                                        
                                    }
                                }*/
                            } else if (hack->type == "dropdown" || hack->value.type == ValueType::Custom) {
                                auto type = obj.get<std::string>("type");
                                if (type == "button") {
                                    if (ImGui::Button(currentLanguage->name(name).c_str())) {
                                        if (name == "Restore Defaults") {
                                            Hacks::processJSON(true);
                                        } else if (name == "Import Theme") {
                                            geode::FileSetting::Filter filter;
                                            filter.description = "Theme (*.json)";
                                            filter.files.insert("*.json");
                                            file::pickFile(
                                                file::PickMode::OpenFile,
                                                {
                                                    dirs::getGameDir(),
                                                    { filter }
                                                },
                                                [&](ghc::filesystem::path path) {
                                                    auto saveDir = Mod::get()->getSaveDir().string();
                                                    if (!ghc::filesystem::exists(saveDir + "/themes")) {
                                                        ghc::filesystem::create_directory(saveDir + "/themes");
                                                    }
                                                    ghc::filesystem::copy_file(path, saveDir + "/themes/" + path.filename().string());
                                                    //ImGui::OpenPopup("Success");
                                                    FLAlertLayer::create("Success!", "The <cy>theme</c> has successfully been imported! Restart your game to use it.", "OK")->show();
                                                }
                                            );
                                        } else if (name == "Reset Speedhack") {
                                            HackItem* speedHack = Hacks::getHack("Speedhack");
                                            speedHack->value.floatValue = 1.0F;
                                            Hacks::Settings::setSettingValue(&settings, *speedHack, 1.0F);
                                            Hacks::setPitch(1.0F);
                                            CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0F);
                                        }
                                        ImGui::OpenPopup(name.c_str());
                                        
                                    }
                                    // Popups
                                    if (ImGui::BeginPopupModal("Credits", nullptr)) {
                                        auto creditLine1 = currentLanguage->name(currentLanguage->name(
"Thank you to Electrify (ES), Jouca (FR), dank_meme01, savvacorgi, МегаСвятой (RU), Gazonk (BR), huhnmitferrari (DE), and Ignis (CS) for translations!"
                                        ));
                                        auto creditLine2 = currentLanguage->name(currentLanguage->name("And thank you for using the mod! I hope you enjoy using Prism Menu!"));
                                        ImGui::Text("%s", creditLine1.c_str());
                                        ImGui::Separator();
                                        ImGui::Text("%s", creditLine2.c_str());
                                        if (ImGui::Button(currentLanguage->name(currentLanguage->name("Close Popup").c_str()).c_str()))
                                            ImGui::CloseCurrentPopup();
                                        ImGui::EndPopup();
                                    }
                                    
                                } else if (type == "dropdown") {
                                    auto values = obj.get<matjson::Array>("values");
                                    if (hack->name == "Theme") {
                                        values = Themes::getCurrentThemes();
                                    }
                                    int selectedIndex = hack->value.intValue;
                                    if (ImGui::BeginCombo(currentLanguage->name(name).c_str(), values[selectedIndex].as_string().c_str())) {
                                        for (size_t i = 0; i < values.size(); i++) {
                                            const bool isSelected = (selectedIndex == i);
                                            if (ImGui::Selectable(values[i].as_string().c_str(), isSelected)) {
                                                hack->value.intValue = i;
                                                Hacks::Settings::setSettingValue(&settings, *hack, hack->value.intValue);
                                                if (name == "Menu Style") {
                                                    showMenu = !showMenu;
                                                }
                                            }
                                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                                            if (isSelected)
                                                ImGui::SetItemDefaultFocus();
                                        }
                                        ImGui::EndCombo();
                                    }
                                }
                            }
                            if (obj.contains("winOnly")) {
                                //#ifndef GEODE_IS_WINDOWS
                                ImGui::EndDisabled();
                                //#endif
                            }
                            if (obj.contains("win")) { // yeah idk what property okay
                                #ifndef GEODE_IS_WINDOWS
                                ImGui::EndDisabled();
                                #endif
                            }
                            if (ImGui::IsItemHovered() && Hacks::isHackEnabled("Show Tooltips")) {
                                if ((obj.contains("winOnly") && Hacks::isHackEnabled("Enable Patching")) || !obj.contains("winOnly")) {
                                    ImGui::BeginTooltip();
                                    ImGui::Text("%s", currentLanguage->desc(name, desc).c_str());
                                    ImGui::EndTooltip();
                                }
                            }
                        }
                    }
                    if (ImGui::Button("X")) {
                        showMenu = !showMenu;
                    }
                    ImGui::PopStyleVar();
                    ImGui::EndTable();
                }
                if (ImGui::BeginPopupModal("Success", nullptr)) {
                    ImGui::Text("The custom theme has successfully been imported! Restart your game to use it.");
                    if (ImGui::Button(currentLanguage->name("Close Popup").c_str()))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                ImGui::End();
            }
        });
            #endif
        
#ifndef GEODE_IS_MACOS
        prismButton = PrismButton::create(CCScene::get());
        prismButton->setVisible(Hacks::isHackEnabled("Show Button"));
        prismButton->setID("prism-icon");
        SceneManager::get()->keepAcrossScenes(prismButton);
#endif
        return true;
    }
};

// maybe this will fix the issue
$execute {
    SettingHackStruct val { matjson::Array() };
    Mod::get()->addCustomSetting<SettingHackValue>("values", val);
}
$on_mod(Loaded) {
    Hacks::processJSON(false);
    Themes::addToCurrentThemes();
    //auto getKeybindHack = Hacks::getHack("Open Menu Keybind");
    //char inputKeybind = 'C';
    //if (getKeybindHack != nullptr) inputKeybind = *(getKeybindHack->value.charValue);
    /*¶ Settings
    BindManager::get()->registerBindable({
        "prism-menu"_spr,
        "Toggle Prism Menu",
        "Keybind for toggling when to show Prism Menu's Mod Menu.",
        { Keybind::create(KEY_C, Modifier::None) },
        "Prism Menu/ModMenu"
    });

    new EventListener([=](InvokeBindEvent* event) {
        if (event->isDown()) {
            // do a backflip!
            showMenu = !showMenu;
        }
        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "prism-menu"_spr));
    */
}

// remove when custom keybinds is fixed
#include <Geode/modify/CCKeyboardDispatcher.hpp>
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool arr) {
        if (down && (key == KEY_Tab)) {
            HackItem* menuStyle = Hacks::getHack("Menu Style");
#ifdef GEODE_IS_MACOS //because no imgui lol
menuStyle->value.intValue = 1;
#endif
            if (menuStyle->value.intValue == 0) { // imgui
                showMenu = !showMenu;
            } else {
                auto prismUIExists = CCScene::get()->getChildByID("prism-menu");
                if (prismUIExists == nullptr) {
                    PrismUI::create()->show();
                } else {
                    static_cast<PrismUI*>(prismUIExists)->onClose(CCNode::create());
                }
            }
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, arr);
    }
};

// sorry, not sorry. for some reason the allHacks vector isnt being initialized in the other .cpp file
// i completely wasted my time writing this whole patch script, and i kinda want android + mac support soooo

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
// TODO later: move these
class $modify(GJBaseGameLayer) {
#ifndef GEODE_IS_MACOS
    // No Mirror Transition, Instant Mirror Portal
    void toggleFlipped(bool p0, bool p1) { // i spent a lot of time figuring out why CCActionTween wont hook, only to realize that p1 instantly transitions it
        if (Hacks::isHackEnabled("Enable Patching")) return GJBaseGameLayer::toggleFlipped(p0, p1);
        /*
pCVar2 = (CCActionInterval *)cocos2d::CCActionTween::create((float)uVar8,(char *)0x3f000000,(float)((ulonglong)uVar8 >> 0x20),fVar7);
        */
        // MOV DWORD PTR [ESP], 3F
        // ??* esp = 0x3F;
        if (!Hacks::isHackEnabled("No Mirror Transition")) GJBaseGameLayer::toggleFlipped(p0, Hacks::isHackEnabled("Instant Mirror Portal"));
    }
#endif
    // Speedhack fix
#ifdef GEODE_IS_WINDOWS // sorry its weird on android
    void applyTimeWarp(float speed) {
        HackItem* speedhack = Hacks::getHack("Speedhack");
        if (speedhack == nullptr) return GJBaseGameLayer::applyTimeWarp(speed);
        if (speedhack->value.floatValue == 1.0F) return GJBaseGameLayer::applyTimeWarp(speed);
        GJBaseGameLayer::applyTimeWarp(speed * speedhack->value.floatValue);
    }
#endif
};
// showing the icon for android users lol
class $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        for (size_t i = 0; i < this->getChildrenCount(); i++) {
            auto obj = this->getChildren()->objectAtIndex(i);
            if (Utils::getNodeName(obj) == "cocos2d::CCMenu") {
                auto menu = static_cast<CCMenu*>(obj);
                auto button = PrismButton::createButton(this);
                button->setPositionX(-240);
                menu->addChild(button);
                break;
            }
        }
        
    }
};

CircleButtonSprite* createCheatIndicator(bool isHacking) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto cheatIndicator = CircleButtonSprite::create(CCNode::create(), (isHacking) ? CircleBaseColor::Pink : CircleBaseColor::Green, CircleBaseSize::Tiny); //, CircleBaseColor::Geode
    cheatIndicator->setAnchorPoint({1,1});
    cheatIndicator->setPosition({winSize.width, winSize.height});
    return cheatIndicator;
}
class $modify(PlayLayer) {
    float previousPositionX = 0.0F;
    GameObject* antiCheatObject; // removing after lol
    CircleButtonSprite* cheatIndicator;
    bool previousTestMode;

    CCSprite* progressBar;
    CCLabelBMFont* percentLabel;
    CCLabelBMFont* attemptLabel;

    bool isCheating = false;
    bool initedDeath = false;
    int updateInit = 0;
    // Anticheat Bypass, Noclip, No Spikes, No Solids
    void destroyPlayer(PlayerObject *p0, GameObject *p1) {
        //bool m_isTestMode = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x413);
        //std::cout << m_isTestMode << std::endl;
        if (Hacks::isHackEnabled("Enable Patching")) return PlayLayer::destroyPlayer(p0, p1);
        bool noclipDisabled = !Hacks::isHackEnabled("No Solids") && !Hacks::isHackEnabled("Noclip");
        if ((noclipDisabled && !Hacks::isHackEnabled("No Spikes"))) return PlayLayer::destroyPlayer(p0, p1);
        if (Hacks::isHackEnabled("Anticheat Bypass")) {
            if (!m_fields->initedDeath) {
            #if !defined(GEODE_IS_ANDROID64) && !defined(GEODE_IS_MACOS)
            if (m_fields->antiCheatObject == nullptr && p1 != nullptr && (
                (p1->m_realXPosition == 0 && p1->m_realYPosition == p0->m_realYPosition) ||
                (p1->m_realXPosition == 0 && p1->m_realYPosition == p0->m_realYPosition) // todo, get player pos during PlayLayer::init
            )) { // thank you whamer100
                m_fields->antiCheatObject = p1;
                m_fields->initedDeath = true;
            } else if (!m_fields->initedDeath && m_fields->antiCheatObject == nullptr) {
                m_fields->antiCheatObject = p1;
                m_fields->initedDeath = true;
            }
            #else //sorry android64 users, someone has to implement fields for GameObject, because I get errors 
                if (!m_fields->initedDeath && m_fields->antiCheatObject == nullptr) {
                    m_fields->antiCheatObject = p1;
                    m_fields->initedDeath = true;
                }
                // bad coding
                /*
                for (int offset = 0x0; ; offset += 0x1) {
                    GameObject* val = reinterpret_cast<GameObject*>(reinterpret_cast<uintptr_t>(this) + offset);
                    if (val == p1) {
                        log::debug("Found Anticheat Object at offset 0x{}", offset);
                        m_fields->antiCheatObject = p1;
                        break;
                    }
                }
                //m_antiCheatObject = p1;
                m_fields->initedDeath = true;*/
            #endif
            }
            if (p1 == m_fields->antiCheatObject) { // nice AC robert
                return PlayLayer::destroyPlayer(p0, p1);
            }
        }
    }
    // Instant Complete, Practice Music, Hide Testmode
    bool init(GJGameLevel *p0, bool p1, bool p2) {
        if (!PlayLayer::init(p0,p1,p2)) return false;
        if (prismButton != nullptr && Hacks::isHackEnabled("Show Button")) prismButton->setVisible(false);
        if (Hacks::isHackEnabled("Instant Complete")) {
            PlayLayer::playEndAnimationToPos({2,2});
        }
        // 0xaa9
        int targetValue = true;
        /*for (int offset = 0x0; offset < 0xAAAA; offset += 0x1) {
            int val = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + offset);
            if (val == 0 || val == 1) {
            //std::cout << "Offset: 0x" << std::hex << offset << std::dec << ", Value: " << val << std::endl;
            }
            /\*if (val == targetValue) {
                std::cout << "Found target " << targetValue << " at offset 0x" << std::hex << offset << std::dec << std::endl;
                break;
            }*\/
        }*/
        #ifndef GEODE_IS_MACOS
        m_fields->previousTestMode = m_isTestMode;
        #endif
        
        if (Hacks::isHackEnabled("Level Edit")) {
            //m_fields->m_gameLevel->m_levelType = static_cast<GJLevelType>(2);
        }
        if (Hacks::isHackEnabled("Hide Testmode")) {
            for (size_t i = 0; i < this->getChildrenCount(); i++) { // i cant test if this works, bruh
                auto obj = this->getChildren()->objectAtIndex(i);
                if (Utils::getNodeName(obj) == "cocos2d::CCLabelBMFont") {
                    auto testModeLabel = static_cast<CCLabelBMFont*>(obj);
                    if (!strcmp(testModeLabel->getString(), "Testmode")) {
                        testModeLabel->setVisible(false);
                        break;
                    }
                }
            }
        }
        if (Hacks::isHackEnabled("Practice Music")) {
            GameStatsManager::sharedState()->toggleEnableItem(UnlockType::GJItem, 17, true);
        }
        m_fields->cheatIndicator = createCheatIndicator(false);
        m_fields->cheatIndicator->setVisible(Hacks::isHackEnabled("Cheat Indicator"));
        this->addChild(m_fields->cheatIndicator);
        return true;
    }
    void onQuit() {
        if (prismButton != nullptr && Hacks::isHackEnabled("Show Button")) prismButton->setVisible(true); // look at this
        m_fields->initedDeath = false;
        PlayLayer::onQuit();
    }
    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);
#ifndef GEODE_IS_MACOS
        if (Hacks::isHackEnabled("Safe Mode")) {
            m_isTestMode = true;
        } else {
            m_isTestMode = m_fields->previousTestMode;
        }
#endif
        // whats the difference between m_fields and not using? i have no idea!
        if ( // i dont know what are considered "cheats"
            Hacks::isHackEnabled("Noclip") ||
            Hacks::isHackEnabled("No Spikes") ||
            Hacks::isHackEnabled("No Solids") ||
            Hacks::isHackEnabled("Freeze Player") ||
            Hacks::isHackEnabled("No Mirror Transition") ||
            Hacks::isHackEnabled("Instant Mirror Portal") ||
            Hacks::isHackEnabled("Jump Hack") ||
            Hacks::isHackEnabled("Instant Complete") ||
            Hacks::isHackEnabled("Force Platformer Mode") ||
            Hacks::isHackEnabled("Change Gravity") ||
            Hacks::isHackEnabled("Layout Mode")
        ) { // cheating
            if (!m_fields->isCheating) {
                m_fields->isCheating = true;
                if (Hacks::isHackEnabled("Cheat Indicator")) {
                    m_fields->cheatIndicator->removeFromParentAndCleanup(true);
                    m_fields->cheatIndicator = createCheatIndicator(true);
                    this->addChild(m_fields->cheatIndicator);
                }
            }
        } else { // not cheating
            if (m_fields->isCheating) {
                m_fields->isCheating = false;
                m_fields->cheatIndicator->removeFromParentAndCleanup(true);
                m_fields->cheatIndicator = createCheatIndicator(false);
                this->addChild(m_fields->cheatIndicator);
            }
        }
        if (Hacks::isHackEnabled("Instant Complete") && m_fields->updateInit < 5) {
            log::debug("CRIMINAL… criminal… criminal… criminal…");
            // funny message
            FLAlertLayer::create(nullptr, "Cheater!", "Just a warning, you will be <cr>banned off leaderboards</c> if you use this on rated levels. Consider this your <cy>warning</c>.", "OK", nullptr)->show();
        }
        m_fields->updateInit = m_fields->updateInit + 1;
        float attemptOpacity = Hacks::getHack("Attempt Opacity")->value.floatValue;
        //if (!Hacks::isHackEnabled("Hide Attempts") && attemptOpacity == 1.0F) return PlayLayer::postUpdate(p0);
        int currentPosition = Hacks::getHack("Progress Bar Position")->value.intValue;
        // stop dynamic_cast abuse
        auto node = typeinfo_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
        if (node == nullptr) return; // never will happen (or will it)
        m_fields->cheatIndicator->setVisible(Hacks::isHackEnabled("Cheat Indicator"));
        if (m_fields->progressBar == nullptr || m_fields->percentLabel == nullptr || m_fields->attemptLabel == nullptr) {
            for (size_t i = 0; i < this->getChildrenCount(); i++) {
                auto obj = this->getChildren()->objectAtIndex(i);
                if (Utils::getNodeName(obj) == "cocos2d::CCLabelBMFont" && m_fields->percentLabel == nullptr) {
                    auto labelTest = static_cast<CCLabelBMFont*>(obj);
                    if (strlen(labelTest->getString()) < 6) {
                        m_fields->percentLabel = labelTest;
                    }
                } else if (Utils::getNodeName(obj) == "cocos2d::CCSprite" && m_fields->progressBar == nullptr) {
                    m_fields->progressBar = static_cast<CCSprite*>(obj);
                }
            }
            for (size_t i = 0; i < node->getChildrenCount(); i++) {
                auto obj = node->getChildren()->objectAtIndex(i);
                if (Utils::getNodeName(obj) == "cocos2d::CCLayer") {
                    auto layer = static_cast<CCLayer*>(obj);
                    for (size_t y = 0; y < layer->getChildrenCount(); y++) {
                        auto obj2 = layer->getChildren()->objectAtIndex(y);
                        if (Utils::getNodeName(obj2) == "cocos2d::CCLabelBMFont") {
                            m_fields->attemptLabel = static_cast<CCLabelBMFont*>(obj2);
                            break;
                        }
                    }
                }
            }
        } else {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            m_fields->progressBar->setRotation(0.0F);
            switch (currentPosition) {
                case 0: // Top (312)
                    m_fields->progressBar->setPositionY(winSize.height - 8);
                    m_fields->percentLabel->setPositionY(winSize.height - 8);
                    break;
                case 1: // Bottom (10)
                    m_fields->progressBar->setPositionY(10);
                    m_fields->percentLabel->setPositionY(10);
                    break;
                case 2: // Left // 275
                    m_fields->progressBar->setPosition({ 10, winSize.height / 2 });
                    m_fields->percentLabel->setPosition({ 5, winSize.height - 45 }); // 275
                    m_fields->progressBar->setRotation(-90.0F);
                    break;
                case 3: // Right
                    m_fields->progressBar->setPosition({ winSize.width - 10, winSize.height / 2 });
                    m_fields->percentLabel->setPosition({ winSize.width - 40, winSize.height - 45 });
                    m_fields->progressBar->setRotation(-90.0F);
                    break;
            }
            m_fields->attemptLabel->setOpacity(attemptOpacity * 255);
        }
    }
    
    // Accurate Percentage
#ifndef GEODE_IS_MACOS
    void updateProgressbar() {
        PlayLayer::updateProgressbar();
        if (Hacks::isHackEnabled("Enable Patching")) return;
        if (Hacks::isHackEnabled("Accurate Percentage")) {
            if (m_fields->percentLabel == nullptr) return;
            float percent = (this->m_player1->getPositionX() / m_levelLength) * 100; // 6
            std::string percentStr = std::to_string(percent) + "%";
            m_fields->percentLabel->setString(percentStr.c_str());
        }
    }
#endif
    void levelComplete() {
        if (!Hacks::isHackEnabled("Safe Mode") || Hacks::isHackEnabled("Enable Patching")) return PlayLayer::levelComplete();
        PlayLayer::resetLevel(); // haha
    }
};

/*
class $modify(PlayLayer) {
    // Accurate Percentage

    /\*void levelComplete() {
        if (!Hacks::isHackEnabled("Safe Mode") || Hacks::isHackEnabled("Enable Patching")) return PlayLayer::levelComplete();
        PlayLayer::resetLevel(); // haha
    }
    void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {
        if (!Hacks::isHackEnabled("Safe Mode")) return PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);
    }*\/ // WHY YOU HAVE DELAY
};
*/





