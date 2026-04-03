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

static void ApplyCoffeeTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = s.FrameRounding = s.GrabRounding = s.TabRounding =
    s.PopupRounding  = s.ChildRounding = s.ScrollbarRounding = 12.0f;
    s.WindowRounding = 18.0f;
    s.WindowPadding  = ImVec2(27, 22);
    s.FramePadding   = ImVec2(21, 10);
    s.ItemSpacing    = ImVec2(15, 12);
    s.ItemInnerSpacing = ImVec2(12, 7);
    s.ScrollbarSize  = 18.0f;
    s.GrabMinSize    = 15.0f;
    s.WindowBorderSize = s.FrameBorderSize = s.PopupBorderSize = 0.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
    c[ImGuiCol_ChildBg]              = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
    c[ImGuiCol_PopupBg]              = ImVec4(0.95f, 0.95f, 0.95f, 0.96f);
    c[ImGuiCol_Text]                 = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_FrameBg]              = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    c[ImGuiCol_FrameBgHovered]       = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    c[ImGuiCol_FrameBgActive]        = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
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
    int idx = 0;
    for (auto it = gToasts.begin(); it != gToasts.end(); ) {
        it->timer -= ImGui::GetIO().DeltaTime;
        if (it->timer <= 0.0f) { it = gToasts.erase(it); continue; }
        float alpha = std::min(it->timer, 1.0f);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 20, y), 0, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(alpha * 0.88f);
        char label[32]; snprintf(label, sizeof(label), "##toast%d", idx++);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 12));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, it->isError
            ? ImVec4(0.90f, 0.30f, 0.30f, 1.0f)
            : ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
        ImGui::Begin(label, nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::TextUnformatted(it->message.c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        y += ImGui::GetWindowHeight() + 8;
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        ++it;
    }
}

enum class Page { Home, Encrypt, Decrypt };
static Page  gCurrentPage = Page::Home;
static float gPageAlpha   = 0.0f;
static Page  gTargetPage  = Page::Home;

static int   gEncFileType = 0, gEncTextMode = 0;
static char  gEncKey[256] = {}, gEncFilename[256] = {}, gEncContent[4096] = {};
static std::string gEncFilePath;
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
    std::string defaultPath = std::string(isImage ? DEC_IMG_DIR : DEC_TEXT_DIR) + "/" + outName;
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
                    PushToast("Text file selected from drag and drop.");
                } else if (IsImageFile(path)) {
                    gEncFileType = 1;
                    gEncFilePath = path;
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
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
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
    for (auto& p : gParticles) {
        p.y -= p.speed * dt;
        if (p.y < -10.0f) { p.y = h + 10.0f; p.x = (float)(rand() % std::max((int)w, 1)); }
        float flicker = p.alpha + 0.05f * std::sin(p.y * 0.05f);
        dl->AddCircleFilled(ImVec2(origin.x + p.x, origin.y + p.y), p.size,
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, flicker)));
    }
}

static void DrawLockIcon(ImDrawList* dl, ImVec2 center, float scale) {
    ImU32 col      = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.35f));
    ImU32 colInner = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.12f));
    float bw = 22.0f * scale, bh = 18.0f * scale;
    ImVec2 bodyTL(center.x - bw * 0.5f, center.y - bh * 0.3f);
    ImVec2 bodyBR(center.x + bw * 0.5f, center.y + bh * 0.7f);
    dl->AddRectFilled(bodyTL, bodyBR, colInner, 4.0f * scale);
    dl->AddRect(bodyTL, bodyBR, col, 4.0f * scale, 0, 1.5f * scale);
    dl->PathArcTo(ImVec2(center.x, bodyTL.y), 10.0f * scale, 3.14159f, 0.0f, 20);
    dl->PathStroke(col, 0, 2.0f * scale);
    dl->AddCircleFilled(ImVec2(center.x, center.y + bh * 0.1f), 2.5f * scale, col);
}

static void DrawHexStream(ImDrawList* dl, ImVec2 pos, float width, float time, float alpha) {
    const char* hexChars = "0123456789ABCDEF";
    ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, alpha));
    float spacing = 27.0f;
    int count = (int)(width / spacing);
    for (int i = 0; i < count; ++i) {
        char ch[2] = { hexChars[((int)(time * 2.0f + i * 3)) % 16], '\0' };
        dl->AddText(ImVec2(pos.x + i * spacing, pos.y + 1.5f * std::sin(time * 1.2f + i * 0.5f)), col, ch);
    }
}

static void RenderHome() {
    ImVec2 avail  = ImGui::GetContentRegionAvail();
    float cx = avail.x, cy = avail.y;
    ImVec2 winPos = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    static float gTime = 0.0f;
    gTime += ImGui::GetIO().DeltaTime;

    if (!gParticlesInit) InitParticles(cx, cy);
    RenderParticles(dl, winPos, cx, cy);

    float cornerLen   = 60.0f;
    float cornerAlpha = 0.12f + 0.05f * std::sin(gTime * 0.8f);
    ImU32 cornerCol   = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, cornerAlpha));
    dl->AddLine(winPos,                                ImVec2(winPos.x + cornerLen, winPos.y),           cornerCol, 1.5f);
    dl->AddLine(winPos,                                ImVec2(winPos.x, winPos.y + cornerLen),           cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y),       ImVec2(winPos.x + cx - cornerLen, winPos.y),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y),       ImVec2(winPos.x + cx, winPos.y + cornerLen),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x, winPos.y + cy),       ImVec2(winPos.x + cornerLen, winPos.y + cy),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x, winPos.y + cy),       ImVec2(winPos.x, winPos.y + cy - cornerLen),      cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y + cy),  ImVec2(winPos.x + cx - cornerLen, winPos.y + cy), cornerCol, 1.5f);
    dl->AddLine(ImVec2(winPos.x + cx, winPos.y + cy),  ImVec2(winPos.x + cx, winPos.y + cy - cornerLen), cornerCol, 1.5f);

    ImGui::Dummy(ImVec2(0, 19));
    DrawLockIcon(dl, ImVec2(winPos.x + cx * 0.5f, winPos.y + 62.0f), 3.3f);
    ImGui::Dummy(ImVec2(0, 60));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
    TextCentered("XOR Cipher Tool", cx);
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 2));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
    TextCentered("File Encryption & Decryption", cx);
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 5));

    ImVec2 hexPos = ImGui::GetCursorScreenPos();
    float hexW = std::min(cx * 0.6f, 510.0f);
    DrawHexStream(dl, ImVec2(hexPos.x + (cx - hexW) * 0.5f, hexPos.y), hexW, gTime, 0.12f);
    ImGui::Dummy(ImVec2(0, 26));

    float bw = 390.0f, bh = 62.0f;
    ImGui::SetCursorPosX((cx - bw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.10f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.30f, 0.30f, 0.30f, 1.0f));
    if (CoffeeButton("[ + ]  Encrypt", ImVec2(bw, bh))) {
        gTargetPage = Page::Encrypt;
        gEncFilePath.clear();
        memset(gEncKey, 0, sizeof(gEncKey));
        memset(gEncFilename, 0, sizeof(gEncFilename));
        memset(gEncContent, 0, sizeof(gEncContent));
        gShowEncKey = false;
    }
    ImGui::PopStyleColor(3);
    ImGui::Dummy(ImVec2(0, 12));
    ImGui::SetCursorPosX((cx - bw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
    if (CoffeeButton("[ - ]  Decrypt", ImVec2(bw, bh))) {
        gTargetPage = Page::Decrypt;
        gDecFileList.clear(); gDecSelected = -1; gDecResult.clear();
        memset(gDecKey, 0, sizeof(gDecKey));
        gShowDecKey = false;
    }
    ImGui::PopStyleColor(3);
    ImGui::Dummy(ImVec2(0, 29));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 0.50f, 0.50f, 1.0f));
    TextCentered(".txt  |  .jpg  |  .jpeg  |  .png", cx);
    ImGui::PopStyleColor();
}

static void RenderEncrypt() {
    if (gProcessing) ImGui::BeginDisabled();
    if (CoffeeButton("<  Back")) gTargetPage = Page::Home;
    if (gProcessing) ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
    ImGui::Text("Encrypt");
    ImGui::PopStyleColor();
    ImGui::Separator(); ImGui::Dummy(ImVec2(0, 7));

    ImGui::Text("File type");
    ImGui::RadioButton("Text File", &gEncFileType, 0); ImGui::SameLine();
    ImGui::RadioButton("Image File", &gEncFileType, 1);
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::TextDisabled("Tip: drag and drop a .txt, .jpg, .jpeg, or .png file into the app window.");
    ImGui::Dummy(ImVec2(0, 5));

    ImGui::Text("Cipher");
    ImGui::RadioButton("XOR", &gEncCipherMethod, 0); ImGui::SameLine();
    ImGui::RadioButton("AES-256", &gEncCipherMethod, 1);
    ImGui::Dummy(ImVec2(0, 7));

    if (gEncFileType == 0) {
        ImGui::Text("Source");
        ImGui::RadioButton("Select existing file", &gEncTextMode, 0); ImGui::SameLine();
        ImGui::RadioButton("Type new content", &gEncTextMode, 1);
        ImGui::Dummy(ImVec2(0, 5));
        if (gEncTextMode == 0) {
            if (CoffeeButton("Browse...")) {
                const char* filters[] = { "*.txt" };
                const char* path = tinyfd_openFileDialog("Select text file", INPUT_DIR.c_str(), 1, filters, "Text files", 0);
                if (path) gEncFilePath = path;
            }
            ImGui::SameLine();
            if (!gEncFilePath.empty()) ImGui::TextWrapped("%s", gEncFilePath.c_str());
            else ImGui::TextDisabled("No file selected");
        } else {
            ImGui::Text("Filename (without .txt)");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##encfname", gEncFilename, sizeof(gEncFilename));
            ImGui::Text("Content");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextMultiline("##enccontent", gEncContent, sizeof(gEncContent), ImVec2(-1, 144));
        }
    } else {
        if (CoffeeButton("Browse Image...")) {
            const char* filters[] = { "*.jpg", "*.jpeg", "*.png" };
            const char* path = tinyfd_openFileDialog("Select image", INPUT_DIR.c_str(), 3, filters, "Image files", 0);
            if (path) gEncFilePath = path;
        }
        ImGui::SameLine();
        if (!gEncFilePath.empty()) ImGui::TextWrapped("%s", gEncFilePath.c_str());
        else ImGui::TextDisabled("No image selected");

        if (gEncFileType == 1 && !gEncFilePath.empty()) {
            LoadPreviewTexture(gEncFilePath, gPreviewTexture, gPreviewW, gPreviewH, gPreviewPath);
            RenderImagePreview(gPreviewTexture, gPreviewW, gPreviewH);
        }
    }

    ImGui::Dummy(ImVec2(0, 7));
    ImGui::Text("Encryption Key");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 80);
    ImGui::InputText("##enckey", gEncKey, sizeof(gEncKey),
        gShowEncKey ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::SmallButton(gShowEncKey ? "Hide" : "Show")) gShowEncKey = !gShowEncKey;
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 12));

    float bw = 330.0f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.10f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.30f, 0.30f, 0.30f, 1.0f));
    if (gProcessing && gOpType == OpType::Encrypt) {
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
        ImGui::ProgressBar(gOpProgress.load(), ImVec2(bw, 29));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
        ImGui::TextDisabled("Encrypting...");
    } else if (!gProcessing && CoffeeButton("Encrypt Now", ImVec2(bw, 58))) {
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
            gProcessing = true;
            gOpProgress = 0.0f;
            gOpType = OpType::Encrypt;
            int ft = gEncFileType, tm = gEncTextMode;
            std::string fp = gEncFilePath, fn(gEncFilename), ct(gEncContent);
            gOpFuture = std::async(std::launch::async, [fp, key, method, ft, tm, fn, ct]() {
                if (ft == 0) {
                    if (tm == 0)
                        gOpResultPath = encryptTextFile(fp, key, method, &gOpProgress);
                    else
                        gOpResultPath = encryptTextFromString(ct, fn, key, method, &gOpProgress);
                } else {
                    gOpResultPath = encryptImageFile(fp, key, method, &gOpProgress);
                }
            });
        }
    }
    ImGui::PopStyleColor(3);
}

static void RenderDecrypt() {
    if (gProcessing) ImGui::BeginDisabled();
    if (CoffeeButton("<  Back")) gTargetPage = Page::Home;
    if (gProcessing) ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
    ImGui::Text("Decrypt");
    ImGui::PopStyleColor();
    ImGui::Separator(); ImGui::Dummy(ImVec2(0, 7));

    ImGui::Text("File type");
    int prevType = gDecFileType;
    ImGui::RadioButton("Text File##d", &gDecFileType, 0); ImGui::SameLine();
    ImGui::RadioButton("Image File##d", &gDecFileType, 1);
    if (gDecFileType != prevType) {
        gDecFileList.clear();
        gDecSelected = -1;
        gDecResult.clear();
        gDecDirectFilePath.clear();
        gDecSavePath.clear();
    }
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::TextDisabled("Tip: drag and drop an encrypted file into the app window.");
    ImGui::Dummy(ImVec2(0, 5));

    ImGui::Text("Cipher");
    ImGui::RadioButton("XOR##d", &gDecCipherMethod, 0); ImGui::SameLine();
    ImGui::RadioButton("AES-256##d", &gDecCipherMethod, 1);
    ImGui::Dummy(ImVec2(0, 5));

    if (CoffeeButton("Refresh List")) {
        gDecFileList = (gDecFileType == 0) ? listEncryptedTextFiles() : listEncryptedImageFiles();
        gDecSelected = -1;
        gDecDirectFilePath.clear();
    }
    ImGui::Dummy(ImVec2(0, 5));

    if (!gDecDirectFilePath.empty()) {
        ImGui::Text("Selected dropped file:");
        ImGui::TextWrapped("%s", gDecDirectFilePath.c_str());
        if (ImGui::SmallButton("Use list selection instead")) gDecDirectFilePath.clear();
        ImGui::Dummy(ImVec2(0, 5));
    }
    if (CoffeeButton("Save As...")) {
        std::string candidatePath;
        if (!gDecDirectFilePath.empty()) {
            candidatePath = gDecDirectFilePath;
        } else if (gDecSelected >= 0 && gDecSelected < (int)gDecFileList.size()) {
            candidatePath = ((gDecFileType == 0) ? ENC_TEXT_DIR : ENC_IMG_DIR) + "/" + gDecFileList[gDecSelected];
        }
        if (candidatePath.empty()) {
            PushToast("Select or drop a file first.", true);
        } else {
            std::string picked = PickDecryptSavePath(candidatePath, gDecFileType == 1);
            if (!picked.empty()) {
                gDecSavePath = picked;
                PushToast("Save destination selected.");
            }
        }
    }
    if (!gDecSavePath.empty()) {
        ImGui::Text("Save destination:");
        ImGui::TextWrapped("%s", gDecSavePath.c_str());
    } else {
        ImGui::TextDisabled("No save destination selected. You will be prompted when decrypting.");
    }
    ImGui::Dummy(ImVec2(0, 5));

    if (gDecFileList.empty()) {
        ImGui::TextDisabled("No encrypted files found. Click Refresh.");
    } else {
        ImGui::Text("Select file to decrypt:");
        ImGui::BeginChild("##filelist", ImVec2(-1, 168), true);
        for (int i = 0; i < (int)gDecFileList.size(); ++i) {
            if (ImGui::Selectable(gDecFileList[i].c_str(), gDecSelected == i))
                gDecSelected = i;
        }
        ImGui::EndChild();
    }

    ImGui::Dummy(ImVec2(0, 7));
    ImGui::Text("Decryption Key");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 80);
    ImGui::InputText("##deckey", gDecKey, sizeof(gDecKey),
        gShowDecKey ? ImGuiInputTextFlags_None : ImGuiInputTextFlags_Password);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (ImGui::SmallButton(gShowDecKey ? "Hide##d" : "Show##d")) gShowDecKey = !gShowDecKey;
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 12));

    float bw = 330.0f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
    if (gProcessing && (gOpType == OpType::DecryptText || gOpType == OpType::DecryptImage)) {
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
        ImGui::ProgressBar(gOpProgress.load(), ImVec2(bw, 29));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
        ImGui::TextDisabled("Decrypting...");
    } else if (!gProcessing && CoffeeButton("Decrypt Now", ImVec2(bw, 58))) {
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
                    PushToast("Decryption cancelled: no save location selected.", true);
                    ImGui::PopStyleColor(3);
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
    ImGui::PopStyleColor(3);

    if (gDecFileType == 0 && !gDecResult.empty()) {
        ImGui::Dummy(ImVec2(0, 12)); ImGui::Separator();
        ImGui::Text("Decrypted Content:");
        ImGui::BeginChild("##decpreview", ImVec2(-1, 168), true);
        ImGui::TextWrapped("%s", gDecResult.c_str());
        ImGui::EndChild();
    }

    if (gDecFileType == 1 && gDecPreviewTexture != 0) {
        RenderImagePreview(gDecPreviewTexture, gDecPreviewW, gDecPreviewH);
    }
}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1350, 900, "XOR Cipher Tool", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSetDropCallback(window, GLFWDropCallback);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig fontCfg; fontCfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&fontCfg);

    ApplyCoffeeTheme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ensureDirectories();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ProcessDroppedFiles();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (gTargetPage != gCurrentPage) {
            gPageAlpha = smoothLerp(gPageAlpha, 0.0f, 10.0f);
            if (gPageAlpha < 0.03f) { gCurrentPage = gTargetPage; gPageAlpha = 0.0f; }
        } else {
            gPageAlpha = smoothLerp(gPageAlpha, 1.0f, 6.0f);
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
        glClearColor(0.97f, 0.97f, 0.97f, 1.0f);
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
