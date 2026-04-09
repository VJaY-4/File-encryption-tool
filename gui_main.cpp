/*
 * gui_main.cpp – Dear ImGui GUI for XOR Cipher Encryption/Decryption Tool
 * Theme: White / black, rounded, minimalistic, subtle animations.
 */

#include "crypto_core.h"
#include "tinyfiledialogs.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <future>
#include <atomic>
#include <chrono>

namespace fs = std::filesystem;

static float smoothLerp(float a, float b, float speed) {
    return a + (b - a) * (1.0f - std::exp(-speed * ImGui::GetIO().DeltaTime));
}

static ImVec4 Accent()    { return ImVec4(0.05f, 0.05f, 0.05f, 1.00f); }
static ImVec4 AccentDim() { return ImVec4(0.45f, 0.45f, 0.45f, 1.00f); }
static ImVec4 AccentSub() { return ImVec4(0.55f, 0.55f, 0.55f, 1.00f); }

static void ApplyTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 10.0f;
    s.FrameRounding = s.GrabRounding = s.TabRounding =
    s.PopupRounding = s.ChildRounding = s.ScrollbarRounding = 6.0f;
    s.WindowPadding  = ImVec2(27, 22);
    s.FramePadding   = ImVec2(21, 10);
    s.ItemSpacing    = ImVec2(15, 12);
    s.ItemInnerSpacing = ImVec2(12, 7);
    s.ScrollbarSize  = 18.0f;
    s.GrabMinSize    = 15.0f;
    s.WindowBorderSize = 0.0f;
    s.FrameBorderSize  = 1.0f;
    s.PopupBorderSize  = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
        c[ImGuiCol_ChildBg]              = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        c[ImGuiCol_PopupBg]              = ImVec4(0.95f, 0.95f, 0.95f, 0.96f);
        c[ImGuiCol_Text]                 = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        c[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        c[ImGuiCol_Border]               = ImVec4(0.70f, 0.70f, 0.70f, 0.35f);
        c[ImGuiCol_FrameBg]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        c[ImGuiCol_FrameBgHovered]       = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        c[ImGuiCol_FrameBgActive]        = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        c[ImGuiCol_TitleBg]              = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        c[ImGuiCol_TitleBgActive]        = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        c[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.92f, 0.92f, 0.92f, 0.75f);
        c[ImGuiCol_Button]               = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        c[ImGuiCol_ButtonHovered]        = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        c[ImGuiCol_ButtonActive]         = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        c[ImGuiCol_Header]               = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        c[ImGuiCol_HeaderHovered]        = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
        c[ImGuiCol_HeaderActive]         = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
        c[ImGuiCol_Tab]                  = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        c[ImGuiCol_TabHovered]           = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        c[ImGuiCol_TabActive]            = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        c[ImGuiCol_TabUnfocused]         = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        c[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        c[ImGuiCol_Separator]            = ImVec4(0.70f, 0.70f, 0.70f, 0.50f);
        c[ImGuiCol_SeparatorHovered]     = ImVec4(0.40f, 0.40f, 0.40f, 0.70f);
        c[ImGuiCol_SeparatorActive]      = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        c[ImGuiCol_ScrollbarBg]          = ImVec4(0.95f, 0.95f, 0.95f, 0.50f);
        c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
        c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        c[ImGuiCol_CheckMark]            = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        c[ImGuiCol_SliderGrab]           = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        c[ImGuiCol_SliderGrabActive]     = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        c[ImGuiCol_ResizeGrip]           = ImVec4(0.60f, 0.60f, 0.60f, 0.40f);
        c[ImGuiCol_ResizeGripHovered]    = ImVec4(0.40f, 0.40f, 0.40f, 0.60f);
        c[ImGuiCol_ResizeGripActive]     = ImVec4(0.20f, 0.20f, 0.20f, 0.90f);
        c[ImGuiCol_NavHighlight]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
}

struct Toast { std::string message; float timer; bool isError; };
static std::vector<Toast> gToasts;

static void PushToast(const std::string& msg, bool isError = false) {
    gToasts.push_back({msg, 3.5f, isError});
}

static void RenderToasts() {
    float y = 20.0f;
    float dt = ImGui::GetIO().DeltaTime;
    float dispX = ImGui::GetIO().DisplaySize.x;
    for (int idx = 0; idx < (int)gToasts.size(); ++idx) {
        auto& t = gToasts[idx];
        t.timer -= dt;
        if (t.timer <= 0.0f) continue;
        float elapsed = 3.5f - t.timer;
        float alpha = std::min({1.0f, elapsed / 0.25f, t.timer});
        ImGui::SetNextWindowPos(ImVec2(dispX - 20, y), 0, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(alpha * 0.88f);
        char label[32]; snprintf(label, sizeof(label), "##toast%d", idx);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 12));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, t.isError
            ? ImVec4(0.60f, 0.10f, 0.10f, 1.0f)
            : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
        ImGui::Begin(label, nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::TextUnformatted(t.message.c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        y += ImGui::GetWindowHeight() + 8;
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
    gToasts.erase(std::remove_if(gToasts.begin(), gToasts.end(),
        [](const Toast& t) { return t.timer <= 0.0f; }), gToasts.end());
}

enum class Page { Home, Encrypt, Decrypt };
static Page  gCurrentPage = Page::Home;
static float gPageAlpha   = 0.0f;
static Page  gTargetPage  = Page::Home;
static ImFont* gTitleFont = nullptr;

static int   gEncFileType = 0, gEncTextMode = 0;
static char  gEncKey[256] = {}, gEncFilename[256] = {}, gEncContent[4096] = {};
static std::string gEncFilePath;
static std::string gEncSavePath;
static bool  gShowEncKey = false;

static int   gDecFileType = 0;
static char  gDecKey[256] = {};
static int   gDecSelected = -1;
static std::vector<std::string> gDecFileList;
static std::string gDecResult;
static std::string gDecDirectFilePath;
static bool  gShowDecKey = false;

static std::vector<std::string> gDroppedPaths;

// Image preview texture
static GLuint gPreviewTexture = 0;
static int    gPreviewW = 0, gPreviewH = 0;
static std::string gPreviewPath;  // tracks which file is currently loaded

static GLuint gDecPreviewTexture = 0;
static int    gDecPreviewW = 0, gDecPreviewH = 0;
static std::string gDecPreviewPath;

static void LoadPreviewTexture(const std::string& path, GLuint& tex, int& tw, int& th, std::string& trackedPath) {
    if (path == trackedPath && tex != 0) return; // already loaded
    if (tex) { glDeleteTextures(1, &tex); tex = 0; }
    trackedPath = path;
    tw = th = 0;
    if (path.empty()) return;
    int channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &tw, &th, &channels, 4);
    if (!data) return;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

static void RenderImagePreview(GLuint tex, int tw, int th) {
    if (!tex || tw <= 0 || th <= 0) return;
    ImGui::Dummy(ImVec2(0, 7));
    ImGui::Text("Preview (%dx%d)", tw, th);
    float maxW = ImGui::GetContentRegionAvail().x;
    float maxH = 420.0f;
    float scale = std::min(maxW / (float)tw, maxH / (float)th);
    if (scale > 1.0f) scale = 1.0f;
    ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(tw * scale, th * scale));
}

static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return s;
}

static bool IsTextFile(const std::string& path) {
    return ToLower(fs::path(path).extension().string()) == ".txt";
}

static bool IsImageFile(const std::string& path) {
    const std::string ext = ToLower(fs::path(path).extension().string());
    return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
}

static std::string BuildEncryptedTextName(const std::string& sourceOrName, bool fromTypedName) {
    std::string name = fromTypedName ? sourceOrName : fs::path(sourceOrName).filename().string();
    if (name.size() < 4 || ToLower(name.substr(name.size() - 4)) != ".txt") name += ".txt";
    return name;
}

static std::string BuildEncryptedImageName(const std::string& sourcePath) {
    return fs::path(sourcePath).stem().string() + ".enc" + fs::path(sourcePath).extension().string();
}

static std::string PickEncryptSavePath(int fileType, int textMode, const std::string& filePath, const std::string& typedName) {
    std::string outName;

    if (fileType == 0) {
        if (textMode == 0) {
            if (filePath.empty()) return std::string();
            outName = BuildEncryptedTextName(filePath, false);
        } else {
            if (typedName.empty()) return std::string();
            outName = BuildEncryptedTextName(typedName, true);
        }
        const std::string defaultPath = outName;
        const char* filters[] = { "*.txt" };
        const char* picked = tinyfd_saveFileDialog("Save encrypted text as", defaultPath.c_str(), 1, filters, "Text files");
        return picked ? std::string(picked) : std::string();
    }

    if (filePath.empty()) return std::string();
    outName = BuildEncryptedImageName(filePath);
    const std::string defaultPath = outName;
    const char* filters[] = { "*.jpg", "*.jpeg", "*.png" };
    const char* picked = tinyfd_saveFileDialog("Save encrypted image as", defaultPath.c_str(), 3, filters, "Image files");
    return picked ? std::string(picked) : std::string();
}

static std::string DecryptedOutputNameFromEncrypted(const std::string& encryptedPath, bool isImage) {
    std::string name = fs::path(encryptedPath).filename().string();
    if (isImage) {
        const size_t pos = name.find(".enc");
        if (pos != std::string::npos) name.erase(pos, 4);
    }
    return name;
}
static std::string PickDecryptSavePath(const std::string& encryptedPath, bool isImage) {
    const std::string outName = DecryptedOutputNameFromEncrypted(encryptedPath, isImage);
    std::string defaultPath = outName;
    if (isImage) {
        const char* filters[] = { "*.jpg", "*.jpeg", "*.png" };
        const char* picked = tinyfd_saveFileDialog("Save decrypted image as", defaultPath.c_str(), 3, filters, "Image files");
        return picked ? std::string(picked) : std::string();
    }
    const char* filters[] = { "*.txt" };
    const char* picked = tinyfd_saveFileDialog("Save decrypted text as", defaultPath.c_str(), 1, filters, "Text files");
    return picked ? std::string(picked) : std::string();
}
static std::string gDecSavePath;

static void GLFWDropCallback(GLFWwindow*, int count, const char** paths) {
    for (int i = 0; i < count; ++i) {
        if (paths[i]) gDroppedPaths.emplace_back(paths[i]);
    }
}

static void ProcessDroppedFiles() {
    if (gDroppedPaths.empty()) return;

    std::vector<std::string> dropped;
    dropped.swap(gDroppedPaths);

    for (const std::string& path : dropped) {
        try {
            if (!fs::exists(path)) {
                PushToast("Dropped file not found.", true);
                continue;
            }
            if (!fs::is_regular_file(path)) {
                PushToast("Only files can be dropped.", true);
                continue;
            }

            if (gCurrentPage == Page::Encrypt) {
                if (IsTextFile(path)) {
                    gEncFileType = 0;
                    gEncTextMode = 0;
                    gEncFilePath = path;
                    gEncSavePath.clear();
                    PushToast("Text file selected from drag and drop.");
                } else if (IsImageFile(path)) {
                    gEncFileType = 1;
                    gEncFilePath = path;
                    gEncSavePath.clear();
                    PushToast("Image file selected from drag and drop.");
                } else {
                    PushToast("Unsupported file. Drop .txt, .jpg, .jpeg, or .png.", true);
                }
                continue;
            }

            if (gCurrentPage == Page::Decrypt) {
                if (IsImageFile(path)) {
                    gDecFileType = 1;
                } else {
                    gDecFileType = 0;
                }
                gDecDirectFilePath = path;
                gDecSelected = -1;
                gDecResult.clear();
                PushToast("File selected for decryption from drag and drop.");
                continue;
            }

            PushToast("Open Encrypt or Decrypt page, then drop files.", true);
        } catch (...) {
            PushToast("Could not process dropped file.", true);
        }
    }
}

// Cipher method: 0 = XOR, 1 = AES-256
static int   gEncCipherMethod = 0;
static int   gDecCipherMethod = 0;

// Async operation state
enum class OpType { None, Encrypt, DecryptText, DecryptImage };
static bool gProcessing = false;
static std::atomic<float> gOpProgress{0.0f};
static std::future<void> gOpFuture;
static std::string gOpResultPath;
static std::string gOpDecContent;
static OpType gOpType = OpType::None;

static void CheckAsyncOp() {
    if (!gProcessing || !gOpFuture.valid()) return;
    if (gOpFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) return;
    try {
        gOpFuture.get();
        PushToast("Saved to: " + gOpResultPath);
        if (gOpType == OpType::DecryptText) gDecResult = gOpDecContent;
        if (gOpType == OpType::DecryptImage) {
            LoadPreviewTexture(gOpResultPath, gDecPreviewTexture, gDecPreviewW, gDecPreviewH, gDecPreviewPath);
        }
    } catch (const std::exception& e) {
        PushToast(e.what(), true);
    }
    gProcessing = false;
    gOpType = OpType::None;
}

static bool CoffeeButton(const char* label, ImVec2 size = ImVec2(0, 0)) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    return clicked;
}

static void TextCentered(const char* text, float regionWidth) {
    ImGui::SetCursorPosX((regionWidth - ImGui::CalcTextSize(text).x) * 0.5f);
    ImGui::TextUnformatted(text);
}

static void SectionLabel(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Text, Accent());
    ImGui::Text("// %s", label);
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 4));
}

static void ThinDivider(float alpha = 0.18f) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    float w = ImGui::GetContentRegionAvail().x;
    ImVec4 ac = Accent();
    ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x + w, p.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(ac.x, ac.y, ac.z, alpha)), 1.0f);
    ImGui::Dummy(ImVec2(0, 1));
}

static void BeginCard() {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 14));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.94f, 0.94f, 0.94f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.78f, 0.78f, 0.78f, 0.40f));
}

static void EndCard() {
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

struct Particle { float x, y, speed, size, alpha; };
static Particle gParticles[80];
static bool gParticlesInit = false;

static void InitParticles(float w, float h) {
    for (auto& p : gParticles) {
        p.x     = (float)(rand() % (int)w);
        p.y     = (float)(rand() % (int)h);
        p.speed = 6.0f + (float)(rand() % 22);
        p.size  = 2.0f + (float)(rand() % 4);
        p.alpha = 0.15f + 0.20f * (float)(rand() % 100) / 100.0f;
    }
    gParticlesInit = true;
}

static void RenderParticles(ImDrawList* dl, ImVec2 origin, float w, float h) {
    float dt = ImGui::GetIO().DeltaTime;
    ImVec4 ac = Accent();
    int wMax = std::max((int)w, 1);
    for (auto& p : gParticles) {
        p.y -= p.speed * dt;
        if (p.y < -10.0f) { p.y = h + 10.0f; p.x = (float)(rand() % wMax); }
        float flicker = (p.alpha + 0.05f * std::sin(p.y * 0.05f)) * 0.5f;
        dl->AddCircleFilled(ImVec2(origin.x + p.x, origin.y + p.y), p.size,
            ImGui::ColorConvertFloat4ToU32(ImVec4(ac.x, ac.y, ac.z, flicker)));
    }
}

static void DrawHexStream(ImDrawList* dl, ImVec2 pos, float width, float time, float alpha) {
    static const char* hexChars = "0123456789ABCDEF";
    ImVec4 ac = Accent();
    ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(ac.x, ac.y, ac.z, alpha));
    constexpr float spacing = 27.0f;
    int count = (int)(width / spacing);
    float baseSin = time * 1.2f;
    int baseIdx = (int)(time * 2.0f);
    for (int i = 0; i < count; ++i) {
        char ch[2] = { hexChars[(baseIdx + i * 3) & 15], '\0' };
        dl->AddText(ImVec2(pos.x + i * spacing, pos.y + 1.5f * std::sin(baseSin + i * 0.5f)), col, ch);
    }
}

static void RenderHome() {
    ImVec2 avail  = ImGui::GetContentRegionAvail();
    float cx = avail.x, cy = avail.y;
    ImVec2 winPos = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    static float gTime = 0.0f;
    gTime += ImGui::GetIO().DeltaTime;

    static float lastW = 0, lastH = 0;
    if (!gParticlesInit || cx != lastW || cy != lastH) {
        InitParticles(cx, cy);
        lastW = cx; lastH = cy;
    }
    RenderParticles(dl, winPos, cx, cy);

    float cornerLen   = 60.0f;
    float cornerAlpha = 0.20f + 0.10f * std::sin(gTime * 0.8f);
    ImVec4 acBase = Accent();
    ImU32 cornerCol = ImGui::ColorConvertFloat4ToU32(ImVec4(acBase.x, acBase.y, acBase.z, cornerAlpha));
    dl->AddLine(winPos,                                ImVec2(winPos.x + cornerLen, winPos.y),           cornerCol, 1.5f);
    dl->AddLine(winPos,                                ImVec2(winPos.x, winPos.y + cornerLen),           cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y),       ImVec2(winPos.x + cx - cornerLen, winPos.y),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y),       ImVec2(winPos.x + cx, winPos.y + cornerLen),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x, winPos.y + cy),       ImVec2(winPos.x + cornerLen, winPos.y + cy),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x, winPos.y + cy),       ImVec2(winPos.x, winPos.y + cy - cornerLen),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y + cy),  ImVec2(winPos.x + cx - cornerLen, winPos.y + cy), cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y + cy),  ImVec2(winPos.x + cx, winPos.y + cy - cornerLen), cornerCol, 1.5f);

    ImGui::Dummy(ImVec2(0, 60));

    ImGui::PushFont(gTitleFont);
    ImGui::PushStyleColor(ImGuiCol_Text, Accent());
    TextCentered("File Encryption Tool", cx);
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0, 6));
    ImGui::PushStyleColor(ImGuiCol_Text, AccentDim());
    TextCentered("Image & Text File Encryption and Decryption", cx);
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 5));

    ImVec2 hexPos = ImGui::GetCursorScreenPos();
    float hexW = std::min(cx * 0.6f, 510.0f);
    DrawHexStream(dl, ImVec2(hexPos.x + (cx - hexW) * 0.5f, hexPos.y), hexW, gTime, 0.25f);
    ImGui::Dummy(ImVec2(0, 26));

    float bw = 390.0f, bh = 62.0f;
    ImGui::SetCursorPosX((cx - bw) * 0.5f);
    if (CoffeeButton("[ + ]  Encrypt", ImVec2(bw, bh))) {
        gTargetPage = Page::Encrypt;
        gEncFilePath.clear();
        gEncSavePath.clear();
        memset(gEncKey, 0, sizeof(gEncKey));
        memset(gEncFilename, 0, sizeof(gEncFilename));
        memset(gEncContent, 0, sizeof(gEncContent));
        gShowEncKey = false;
    }
    ImGui::Dummy(ImVec2(0, 12));
    ImGui::SetCursorPosX((cx - bw) * 0.5f);
    if (CoffeeButton("[ - ]  Decrypt", ImVec2(bw, bh))) {
        gTargetPage = Page::Decrypt;
        gDecFileList.clear(); gDecSelected = -1; gDecResult.clear();
        memset(gDecKey, 0, sizeof(gDecKey));
        gShowDecKey = false;
    }
    ImGui::Dummy(ImVec2(0, 29));

    // --- feature card ---
    {
        float cardW = std::min(cx * 0.65f, 520.0f);
        ImGui::SetCursorPosX((cx - cardW) * 0.5f);
        BeginCard();
        ImGui::BeginChild("##features", ImVec2(cardW, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleColor(ImGuiCol_Text, Accent());
        ImGui::Text("  Features");
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 2));
        ThinDivider(0.25f);
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::PushStyleColor(ImGuiCol_Text, AccentDim());
        ImGui::BulletText("XOR & AES-256 cipher support");
        ImGui::BulletText("Text file & image encryption");
        ImGui::BulletText("Drag & drop file selection");
        ImGui::BulletText("Native Save As dialogs");
        ImGui::BulletText("Dark / light theme ready");
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::EndChild();
        EndCard();
    }
    ImGui::Dummy(ImVec2(0, 20));

    ImGui::PushStyleColor(ImGuiCol_Text, AccentSub());
    TextCentered(".txt  |  .jpg  |  .jpeg  |  .png", cx);
    ImGui::PopStyleColor();

    // --- footer ---
    {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetStyle().WindowPadding.y - ImGui::GetTextLineHeight() - 4);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.70f, 0.70f, 0.70f, 1.0f));
        ImGui::SetWindowFontScale(0.70f);
        TextCentered("v1.0   |   C++17   |   OpenGL 3.3   |   Dear ImGui", cx);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
    }
}

static void RenderEncrypt() {
    // === HEADER (pinned) ===
    if (gProcessing) ImGui::BeginDisabled();
    if (CoffeeButton("<  Back")) gTargetPage = Page::Home;
    if (gProcessing) ImGui::EndDisabled();
    ImGui::SameLine();
    {
        ImGui::PushStyleColor(ImGuiCol_Text, Accent());
        float tw = ImGui::CalcTextSize("ENCRYPT").x;
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - tw + ImGui::GetCursorPosX());
        ImGui::Text("ENCRYPT");
        ImGui::PopStyleColor();
    }
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 4));

    // === Scrollable body ===
    ImGui::BeginChild("##enc_scroll", ImVec2(0, 0), false);

    // === // CONFIGURATION ===
    BeginCard();
    ImGui::BeginChild("##enc_cfg_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    SectionLabel("CONFIGURATION");
    {
        int prevEncType = gEncFileType;
        ImGui::Text("Type"); ImGui::SameLine(80);
        ImGui::RadioButton("Text", &gEncFileType, 0); ImGui::SameLine();
        ImGui::RadioButton("Image", &gEncFileType, 1);
        if (gEncFileType != prevEncType) gEncSavePath.clear();
        ImGui::Text("Cipher"); ImGui::SameLine(80);
        ImGui::RadioButton("XOR", &gEncCipherMethod, 0); ImGui::SameLine();
        ImGui::RadioButton("AES-256", &gEncCipherMethod, 1);
    }
    if (gEncFileType == 0) {
        int prevTextMode = gEncTextMode;
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::Text("Source"); ImGui::SameLine(80);
        ImGui::RadioButton("File", &gEncTextMode, 0); ImGui::SameLine();
        ImGui::RadioButton("New Content", &gEncTextMode, 1);
        if (gEncTextMode != prevTextMode) gEncSavePath.clear();
    }
    ImGui::EndChild();
    EndCard();
    ImGui::Dummy(ImVec2(0, 10));
    ThinDivider();
    ImGui::Dummy(ImVec2(0, 10));

    // === // INPUT ===
    BeginCard();
    ImGui::BeginChild("##enc_input_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    SectionLabel("INPUT");
    if (gEncFileType == 0 && gEncTextMode == 0) {
        if (CoffeeButton("Browse File...")) {
            const char* filters[] = { "*.txt" };
            const char* path = tinyfd_openFileDialog("Select text file", "", 1, filters, "Text files", 0);
            if (path) { gEncFilePath = path; gEncSavePath.clear(); }
        }
        ImGui::SameLine();
        if (!gEncFilePath.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, AccentDim());
            ImGui::TextWrapped("> %s", gEncFilePath.c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::TextDisabled("> no file selected");
        }
    } else if (gEncFileType == 0 && gEncTextMode == 1) {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::InputText("##encfname", gEncFilename, sizeof(gEncFilename));
        ImGui::SameLine(); ImGui::TextDisabled(".txt");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextMultiline("##enccontent", gEncContent, sizeof(gEncContent), ImVec2(-1, 160));
    } else {
        if (CoffeeButton("Browse Image...")) {
            const char* filters[] = { "*.jpg", "*.jpeg", "*.png" };
            const char* path = tinyfd_openFileDialog("Select image", "", 3, filters, "Image files", 0);
            if (path) { gEncFilePath = path; gEncSavePath.clear(); }
        }
        ImGui::SameLine();
        if (!gEncFilePath.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, AccentDim());
            ImGui::TextWrapped("> %s", gEncFilePath.c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::TextDisabled("> no image selected");
        }
        if (!gEncFilePath.empty()) {
            LoadPreviewTexture(gEncFilePath, gPreviewTexture, gPreviewW, gPreviewH, gPreviewPath);
            RenderImagePreview(gPreviewTexture, gPreviewW, gPreviewH);
        }
    }
    ImGui::EndChild();
    EndCard();
    ImGui::Dummy(ImVec2(0, 10));
    ThinDivider();
    ImGui::Dummy(ImVec2(0, 10));

    // === // KEY ===
    BeginCard();
    ImGui::BeginChild("##enc_key_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    SectionLabel("KEY");
    float encInputH = ImGui::GetFrameHeight();
    ImGui::SetNextItemWidth((ImGui::GetContentRegionAvail().x - 60) * 0.75f);
    ImGui::InputText("##enckey", gEncKey, sizeof(gEncKey),
        gShowEncKey ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password);
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SetWindowFontScale(0.75f);
    if (ImGui::Button(gShowEncKey ? "Hide" : "Show", ImVec2(48, encInputH))) gShowEncKey = !gShowEncKey;
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::Dummy(ImVec2(0, 4));
    ImGui::TextDisabled("// drag & drop supported files into this window");
    ImGui::EndChild();
    EndCard();
    ImGui::Dummy(ImVec2(0, 14));

    // === ACTION ===
    float bw = ImGui::GetContentRegionAvail().x;
    if (gProcessing && gOpType == OpType::Encrypt) {
        ImGui::ProgressBar(gOpProgress.load(), ImVec2(bw, 32));
        ImGui::TextDisabled("Encrypting...");
    } else if (!gProcessing && CoffeeButton(">>>  ENCRYPT NOW  <<<", ImVec2(bw, 52))) {
        std::string key(gEncKey);
        if (key.empty()) {
            PushToast("Enter an encryption key.", true);
        } else if (gEncFileType == 0 && gEncTextMode == 0 && gEncFilePath.empty()) {
            PushToast("Select a file first.", true);
        } else if (gEncFileType == 0 && gEncTextMode == 0 && !fs::exists(gEncFilePath)) {
            PushToast("Selected file does not exist.", true);
        } else if (gEncFileType == 0 && gEncTextMode == 1 && strlen(gEncFilename) == 0) {
            PushToast("Enter a filename.", true);
        } else if (gEncFileType == 0 && gEncTextMode == 1 && strlen(gEncContent) == 0) {
            PushToast("Enter some content to encrypt.", true);
        } else if (gEncFileType == 1 && gEncFilePath.empty()) {
            PushToast("Select an image first.", true);
        } else if (gEncFileType == 1 && !fs::exists(gEncFilePath)) {
            PushToast("Selected image does not exist.", true);
        } else {
            CipherMethod method = (gEncCipherMethod == 1) ? CipherMethod::AES256 : CipherMethod::XOR;
            std::string savePath = gEncSavePath;
            if (savePath.empty()) {
                savePath = PickEncryptSavePath(gEncFileType, gEncTextMode, gEncFilePath, gEncFilename);
            }
            if (savePath.empty()) {
                PushToast("Encryption cancelled.", true);
                ImGui::EndChild(); // end enc_scroll before early return
                return;
            }
            gProcessing = true;
            gOpProgress = 0.0f;
            gOpType = OpType::Encrypt;
            int ft = gEncFileType, tm = gEncTextMode;
            std::string fp = gEncFilePath, fn(gEncFilename), ct(gEncContent);
            gEncSavePath = savePath;
            gOpFuture = std::async(std::launch::async, [fp, key, method, ft, tm, fn, ct, savePath]() {
                if (ft == 0) {
                    if (tm == 0)
                        gOpResultPath = encryptTextFile(fp, key, savePath, method, &gOpProgress);
                    else
                        gOpResultPath = encryptTextFromString(ct, fn, key, savePath, method, &gOpProgress);
                } else {
                    gOpResultPath = encryptImageFile(fp, key, savePath, method, &gOpProgress);
                }
            });
        }
    }

    ImGui::EndChild(); // end enc_scroll
}

static void RenderDecrypt() {
    // === HEADER (pinned) ===
    if (gProcessing) ImGui::BeginDisabled();
    if (CoffeeButton("<  Back")) gTargetPage = Page::Home;
    if (gProcessing) ImGui::EndDisabled();
    ImGui::SameLine();
    {
        ImGui::PushStyleColor(ImGuiCol_Text, Accent());
        float tw = ImGui::CalcTextSize("DECRYPT").x;
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - tw + ImGui::GetCursorPosX());
        ImGui::Text("DECRYPT");
        ImGui::PopStyleColor();
    }
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 4));

    // === Scrollable body ===
    ImGui::BeginChild("##dec_scroll", ImVec2(0, 0), false);

    // === // CONFIGURATION ===
    BeginCard();
    ImGui::BeginChild("##dec_cfg_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    SectionLabel("CONFIGURATION");
    {
        int prevType = gDecFileType;
        ImGui::Text("Type"); ImGui::SameLine(80);
        ImGui::RadioButton("Text##d", &gDecFileType, 0); ImGui::SameLine();
        ImGui::RadioButton("Image##d", &gDecFileType, 1);
        ImGui::Text("Cipher"); ImGui::SameLine(80);
        ImGui::RadioButton("XOR##d", &gDecCipherMethod, 0); ImGui::SameLine();
        ImGui::RadioButton("AES-256##d", &gDecCipherMethod, 1);
        if (gDecFileType != prevType) {
            gDecFileList.clear(); gDecSelected = -1;
            gDecResult.clear(); gDecDirectFilePath.clear(); gDecSavePath.clear();
        }
    }
    ImGui::EndChild();
    EndCard();
    ImGui::Dummy(ImVec2(0, 10));
    ThinDivider();
    ImGui::Dummy(ImVec2(0, 10));

    // === // FILE ===
    BeginCard();
    ImGui::BeginChild("##dec_file_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    SectionLabel("FILE");
    if (!gDecDirectFilePath.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, AccentDim());
        ImGui::TextWrapped("> %s", gDecDirectFilePath.c_str());
        ImGui::PopStyleColor();
        if (ImGui::SmallButton("Clear")) gDecDirectFilePath.clear();
        ImGui::Dummy(ImVec2(0, 4));
    }
    if (CoffeeButton("Refresh List")) {
        gDecFileList = (gDecFileType == 0) ? listEncryptedTextFiles() : listEncryptedImageFiles();
        gDecSelected = -1;
        gDecDirectFilePath.clear();
    }
    ImGui::SameLine();
    ImGui::TextDisabled("// or drag & drop encrypted files");
    ImGui::Dummy(ImVec2(0, 4));

    if (gDecFileList.empty()) {
        ImGui::TextDisabled("> no encrypted files found");
    } else {
        float listH = std::min(145.0f, (float)gDecFileList.size() * ImGui::GetTextLineHeightWithSpacing() + 12.0f);
        ImGui::BeginChild("##filelist", ImVec2(-1, listH), true);
        for (int i = 0; i < (int)gDecFileList.size(); ++i) {
            if (ImGui::Selectable(gDecFileList[i].c_str(), gDecSelected == i))
                gDecSelected = i;
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
    EndCard();
    ImGui::Dummy(ImVec2(0, 10));
    ThinDivider();
    ImGui::Dummy(ImVec2(0, 10));

    // === // KEY ===
    BeginCard();
    ImGui::BeginChild("##dec_key_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    SectionLabel("KEY");
    float decInputH = ImGui::GetFrameHeight();
    ImGui::SetNextItemWidth((ImGui::GetContentRegionAvail().x - 60) * 0.75f);
    ImGui::InputText("##deckey", gDecKey, sizeof(gDecKey),
        gShowDecKey ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password);
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SetWindowFontScale(0.75f);
    if (ImGui::Button(gShowDecKey ? "Hide##d" : "Show##d", ImVec2(48, decInputH))) gShowDecKey = !gShowDecKey;
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
    EndCard();
    ImGui::Dummy(ImVec2(0, 14));

    // === ACTION ===
    float bw = ImGui::GetContentRegionAvail().x;
    if (gProcessing && (gOpType == OpType::DecryptText || gOpType == OpType::DecryptImage)) {
        ImGui::ProgressBar(gOpProgress.load(), ImVec2(bw, 32));
        ImGui::TextDisabled("Decrypting...");
    } else if (!gProcessing && CoffeeButton("<<<  DECRYPT NOW  >>>", ImVec2(bw, 52))) {
        std::string key(gDecKey);
        if (key.empty()) {
            PushToast("Enter a decryption key.", true);
        } else if (gDecSelected < 0 && gDecDirectFilePath.empty()) {
            PushToast("Select or drop a file to decrypt.", true);
        } else {
            CipherMethod method = (gDecCipherMethod == 1) ? CipherMethod::AES256 : CipherMethod::XOR;
            std::string fullPath;
            if (!gDecDirectFilePath.empty()) {
                fullPath = gDecDirectFilePath;
            } else {
                fullPath = ((gDecFileType == 0) ? ENC_TEXT_DIR : ENC_IMG_DIR)
                         + "/" + gDecFileList[gDecSelected];
            }
            if (!fs::exists(fullPath)) {
                PushToast("File no longer exists. Refresh the list.", true);
            } else {
                std::string savePath = gDecSavePath;
                if (savePath.empty()) {
                    savePath = PickDecryptSavePath(fullPath, gDecFileType == 1);
                }
                if (savePath.empty()) {
                    PushToast("Decryption cancelled.", true);
                    ImGui::EndChild(); // end dec_scroll before early return
                    return;
                }
                gProcessing = true;
                gOpProgress = 0.0f;
                int ft = gDecFileType;
                gOpType = (ft == 0) ? OpType::DecryptText : OpType::DecryptImage;
                gDecSavePath = savePath;
                gOpFuture = std::async(std::launch::async, [fullPath, key, method, ft, savePath]() {
                    if (ft == 0) {
                        std::string savedPath;
                        gOpDecContent = decryptTextFile(fullPath, key, savedPath, savePath, method, &gOpProgress);
                        gOpResultPath = savedPath;
                    } else {
                        gOpResultPath = decryptImageFile(fullPath, key, savePath, method, &gOpProgress);
                    }
                });
            }
        }
    }

    // === // OUTPUT ===
    if (gDecFileType == 0 && !gDecResult.empty()) {
        ImGui::Dummy(ImVec2(0, 10));
        ThinDivider();
        ImGui::Dummy(ImVec2(0, 10));
        BeginCard();
        ImGui::BeginChild("##dec_out_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        SectionLabel("OUTPUT");
        ImGui::BeginChild("##decpreview", ImVec2(-1, 160), true);
        ImGui::TextWrapped("%s", gDecResult.c_str());
        ImGui::EndChild();
        ImGui::EndChild();
        EndCard();
    }
    if (gDecFileType == 1 && gDecPreviewTexture != 0) {
        ImGui::Dummy(ImVec2(0, 10));
        ThinDivider();
        ImGui::Dummy(ImVec2(0, 10));
        BeginCard();
        ImGui::BeginChild("##dec_imgout_card", ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        SectionLabel("OUTPUT");
        RenderImagePreview(gDecPreviewTexture, gDecPreviewW, gDecPreviewH);
        ImGui::EndChild();
        EndCard();
    }

    ImGui::EndChild(); // end dec_scroll
}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1350, 900, "File Encryption Tool", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSetDropCallback(window, GLFWDropCallback);
    glfwSwapInterval(1);
    glEnable(GL_MULTISAMPLE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig fontCfg; fontCfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&fontCfg);

    ImFontConfig titleCfg; titleCfg.SizePixels = 42.0f;
    gTitleFont = io.Fonts->AddFontDefault(&titleCfg);

    ApplyTheme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glClearColor(0.97f, 0.97f, 0.97f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        // Smart frame pacing: only spin at full rate when animating
        bool animating = (gCurrentPage == Page::Home)
                      || (gTargetPage != gCurrentPage)
                      || gProcessing
                      || !gToasts.empty();
        if (animating)
            glfwPollEvents();
        else
            glfwWaitEventsTimeout(0.1);

        ProcessDroppedFiles();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (gTargetPage != gCurrentPage) {
            gPageAlpha = smoothLerp(gPageAlpha, 0.0f, 8.0f);
            if (gPageAlpha < 0.01f) { gCurrentPage = gTargetPage; gPageAlpha = 0.0f; }
        } else {
            gPageAlpha = smoothLerp(gPageAlpha, 1.0f, 8.0f);
        }

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        int w, h; glfwGetFramebufferSize(window, &w, &h);
        ImGui::SetNextWindowSize(ImVec2((float)w, (float)h));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gPageAlpha);
        ImGui::Begin("##main", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        switch (gCurrentPage) {
            case Page::Home:    RenderHome();    break;
            case Page::Encrypt: RenderEncrypt(); break;
            case Page::Decrypt: RenderDecrypt(); break;
        }

        ImGui::End();
        ImGui::PopStyleVar();
        CheckAsyncOp();
        RenderToasts();

        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
