#include "pch.h"
#include "Text.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "Shader.h"

Text::Text() : Super(ComponentType::Text)
{
}

Text::~Text()
{
}
void Text::Init()
{
    auto go = GetGameObject();

    if (go->GetMeshRenderer() == nullptr) {
        go->AddComponent(make_shared<MeshRenderer>());
    }

    m_mesh = make_shared<Mesh>();
    m_mesh->CreateQuad();

    m_material = make_shared<Material>();
    auto shader = make_shared<Shader>(L"TextShader.fx");
    m_material->SetShader(shader);
    m_material->SetRenderQueue(RenderQueue::Transparent);

    // 셰이더 변수 가져오기
    m_textColorEffect = shader->GetVector("TextColor");
    m_outlineColorEffect = shader->GetVector("OutlineColor");
    m_textAlphaEffect = shader->GetScalar("TextAlpha");
    m_outlineWidthEffect = shader->GetScalar("OutlineWidth");

    go->GetMeshRenderer()->SetMesh(m_mesh);
    go->GetMeshRenderer()->SetMaterial(m_material);
    go->GetMeshRenderer()->SetPass(0);
    go->SetLayerIndex(LAYER_UI);

    CreateTextTexture();
}


void Text::Update()
{
    Super::Update();

    if (m_needUpdate) {
        CreateTextTexture();
        UpdateMaterial();
        PushTextData();
        m_needUpdate = false;
    }
}

void Text::SetText(const wstring& text)
{
    if (m_text != text) {
        m_text = text;
        m_needUpdate = true;
    }
}

void Text::SetColor(const Vec4& color)
{
    if (m_color != color) {
        m_color = color;
        m_needUpdate = true;
    }
}

void Text::SetPosition(const Vec2& position)
{
    m_position = position;

    auto go = GetGameObject();
    if (go) {
        float height = GRAPHICS->GetViewport().GetHeight();
        float width = GRAPHICS->GetViewport().GetWidth();

        float x = position.x - width / 2.0f;
        float y = height / 2.0f - position.y;

        go->GetTransform()->SetPosition(Vec3(x, y, 0));
    }
}

void Text::SetFontSize(float size)
{
    if (m_fontSize != size) {
        m_fontSize = size;
        m_needUpdate = true;
    }
}

void Text::SetFontName(const wstring& fontName)
{
    if (m_fontName != fontName) {
        m_fontName = fontName;
        m_needUpdate = true;
    }
}

void Text::SetAlpha(float alpha)
{
    if (m_alpha != alpha) {
        m_alpha = clamp(alpha, 0.0f, 1.0f);
        m_needUpdate = true;
    }
}

// 외곽선 관련 함수들 추가
void Text::SetOutlineColor(const Vec4& color)
{
    if (m_outlineColor != color) {
        m_outlineColor = color;
        m_needUpdate = true;
    }
}

void Text::SetOutlineWidth(float width)
{
    if (m_outlineWidth != width) {
        m_outlineWidth = clamp(width, 0.0f, 5.0f);
        m_needUpdate = true;
    }
}

void Text::EnableOutline(bool enable)
{
    if (m_enableOutline != enable) {
        m_enableOutline = enable;
        m_needUpdate = true;
    }
}


void Text::Create(Vec2 screenPos, const wstring& text, float fontSize,
    Vec4 color, float alpha, Vec4 outlineColor, float outlineWidth)
{
    if (!m_material) {
        Init();
    }

    m_text = text;
    m_fontSize = fontSize;
    m_color = color;
    m_alpha = alpha;
    m_outlineColor = outlineColor;
    m_outlineWidth = outlineWidth;
    m_position = screenPos;

    SetPosition(screenPos);

    auto go = GetGameObject();
    go->GetTransform()->SetScale(Vec3(1, 1, 1));

    m_needUpdate = true;
}

void Text::CreateTextTexture()
{
    if (m_text.empty()) return;

    // 최소 텍스처 크기 보장
    int minWidth = 64;
    int minHeight = 32;

    // Windows GDI를 사용한 텍스트 렌더링
    HDC hdc = CreateCompatibleDC(NULL);
    if (!hdc) return;

    // 폰트 생성
    int fontHeight = static_cast<int>(m_fontSize * 1.5f);
    HFONT hFont = CreateFont(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        m_fontName.c_str()
    );

    if (!hFont) {
        DeleteDC(hdc);
        return;
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    // 텍스트 크기 측정
    SIZE textSize;
    GetTextExtentPoint32(hdc, m_text.c_str(), static_cast<int>(m_text.length()), &textSize);

    // 텍스트 크기에 맞게 설정
    m_textWidth = max(textSize.cx + 8, minWidth);
    m_textHeight = max(textSize.cy + 8, minHeight);

    // 32비트 DIB 섹션 생성
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_textWidth;
    bmi.bmiHeader.biHeight = -m_textHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    if (!hBitmap || !pBits) {
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
        DeleteDC(hdc);
        return;
    }

    HBITMAP oldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);

    // 배경을 완전히 투명하게 초기화
    BYTE* pixels = static_cast<BYTE*>(pBits);
    memset(pixels, 0, m_textWidth * m_textHeight * 4);

    // 텍스트 렌더링 설정 (흰색으로 렌더링)
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    // 텍스트를 중앙에 그리기
    int textX = (m_textWidth - textSize.cx) / 2;
    int textY = (m_textHeight - textSize.cy) / 2;
    TextOut(hdc, textX, textY, m_text.c_str(), static_cast<int>(m_text.length()));

    // 픽셀 데이터 처리 (BGRA -> RGBA 변환 및 알파 채널 설정)
    for (int i = 0; i < m_textWidth * m_textHeight; ++i) {
        BYTE b = pixels[i * 4 + 0];
        BYTE g = pixels[i * 4 + 1];
        BYTE r = pixels[i * 4 + 2];

        // 루미넌스 계산으로 텍스트 픽셀 감지
        float luminance = (r * 0.299f + g * 0.587f + b * 0.114f) / 255.0f;

        if (luminance > 0.1f) { // 텍스트 픽셀
            pixels[i * 4 + 0] = 255; // R (흰색)
            pixels[i * 4 + 1] = 255; // G (흰색)
            pixels[i * 4 + 2] = 255; // B (흰색)
            pixels[i * 4 + 3] = 255; // A (불투명)
        }
        else { // 배경 픽셀
            pixels[i * 4 + 0] = 0; // R
            pixels[i * 4 + 1] = 0; // G
            pixels[i * 4 + 2] = 0; // B
            pixels[i * 4 + 3] = 0; // A (완전 투명)
        }
    }

    // DirectX 텍스처 생성
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = m_textWidth;
    desc.Height = m_textHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixels;
    initData.SysMemPitch = m_textWidth * 4;

    ComPtr<ID3D11Texture2D> texture2D;
    HRESULT hr = DEVICE->CreateTexture2D(&desc, &initData, texture2D.GetAddressOf());

    if (SUCCEEDED(hr)) {
        ComPtr<ID3D11ShaderResourceView> srv;
        hr = DEVICE->CreateShaderResourceView(texture2D.Get(), nullptr, srv.GetAddressOf());

        if (SUCCEEDED(hr)) {
            m_textTexture = make_shared<Texture>();
            m_textTexture->SetSRV(srv);

            // Transform 스케일 조정
            auto go = GetGameObject();
            if (go) {
                float scaleX = static_cast<float>(m_textWidth);
                float scaleY = static_cast<float>(m_textHeight);
                go->GetTransform()->SetScale(Vec3(scaleX, scaleY, 1.0f));
            }
        }
    }

    // 정리
    SelectObject(hdc, oldBitmap);
    SelectObject(hdc, oldFont);
    DeleteObject(hBitmap);
    DeleteObject(hFont);
    DeleteDC(hdc);
}

void Text::UpdateMaterial()
{
    if (m_material && m_textTexture) {
        m_material->SetDiffuseMap(m_textTexture);

        // 머티리얼 색상 설정
        MaterialDesc& desc = m_material->GetMaterialDesc();
        desc.ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        desc.diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        desc.specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        desc.emissive = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void Text::PushTextData()
{
    // 셰이더에 데이터 전달
    if (m_textColorEffect) {
        float colorArray[4] = { m_color.x, m_color.y, m_color.z, m_color.w };
        m_textColorEffect->SetFloatVector(colorArray);
    }

    if (m_outlineColorEffect) {
        float outlineArray[4] = { m_outlineColor.x, m_outlineColor.y, m_outlineColor.z, m_outlineColor.w };
        m_outlineColorEffect->SetFloatVector(outlineArray);
    }

    if (m_textAlphaEffect) {
        m_textAlphaEffect->SetFloat(m_alpha);
    }

    if (m_outlineWidthEffect) {
        m_outlineWidthEffect->SetFloat(m_enableOutline ? m_outlineWidth : 0.0f);
    }
}