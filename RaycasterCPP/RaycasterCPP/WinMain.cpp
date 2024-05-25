#include <d3d11.h>
#include <DirectXMath.h>

// Define the vertex structure
struct Vertex
{
    DirectX::XMFLOAT3 position;
};

// Pixel shader code
const char* pixelShaderCode = R"(
    float4 main() : SV_TARGET
    {
        return float4(1.0f, 0.0f, 0.0f, 1.0f); // Red pixel
    }
)";

// Main function
int __stdcall WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    // Create a window

    // Initialize DirectX (device, swap chain, context, etc.)

    // Define vertices for the quad (for simplicity, we'll draw a pixel at (100, 100))
    Vertex vertices[] =
    {
        { DirectX::XMFLOAT3(100.0f, 100.0f, 0.0f) }, // Top left
        { DirectX::XMFLOAT3(101.0f, 100.0f, 0.0f) }, // Top right
        { DirectX::XMFLOAT3(100.0f, 101.0f, 0.0f) }, // Bottom left
        { DirectX::XMFLOAT3(101.0f, 101.0f, 0.0f) }  // Bottom right
    };

    // Create vertex buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    ID3D11Buffer* vertexBuffer;
    device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);

    // Create pixel shader
    ID3D11PixelShader* pixelShader;
    device->CreatePixelShader(pixelShaderCode, strlen(pixelShaderCode), nullptr, &pixelShader);

    // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Set pixel shader
    context->PSSetShader(pixelShader, nullptr, 0);

    // Draw the quad (2 triangles)
    context->Draw(6, 0);

    // Present the frame

    // Cleanup resources

    return 0;
}
