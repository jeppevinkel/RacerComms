#pragma once
#include <windows.h>
#include <d2d1.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1")

#include "BaseWindow.h"

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class MainWindow : public BaseWindow<MainWindow>
{
	ID2D1Factory* pFactory;
	IDWriteFactory* pWriteFactory;
	IDWriteTextFormat* pTextFormat;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;
	D2D1_ELLIPSE            ellipse;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	HRESULT    CreateDeviceIndependentResources();
	void    DiscardDeviceIndependentResources();
	void    OnPaint();
	void    Resize();

public:

	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
	{
	}

	RECT rect;

	PCWSTR  ClassName() const { return L"Circle Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y);
		ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

HRESULT MainWindow::CreateDeviceIndependentResources()
{
	static const WCHAR msc_fontName[] = L"Verdana";
	static const FLOAT msc_fontSize = 50;
	HRESULT hr;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);

	if (SUCCEEDED(hr))
	{

		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pWriteFactory),
			reinterpret_cast<IUnknown * *>(&pFactory)
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = pWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


	}

	return hr;
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();

	HRESULT hr2;

	hr2 = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr2))
	{
		static const WCHAR sc_helloWorld[] = L"Hello, World!";

		// Retrieve the size of the render target.
		D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

		pRenderTarget->BeginDraw();

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		pRenderTarget->DrawText(
			sc_helloWorld,
			ARRAYSIZE(sc_helloWorld) - 1,
			pTextFormat,
			D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
			pBrush
		);

		hr = pRenderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			//DiscardDeviceResources();
		}
	}
	/*if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		pRenderTarget->FillEllipse(ellipse, pBrush);
		//pRenderTarget->

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		BeginPaint(m_hwnd, &ps);

		rect.top = 0;
		rect.left = 0;
		rect.right = 200;
		rect.bottom = 300;

		HDC hdc = BeginPaint(m_hwnd, &ps);
		GetClientRect(m_hwnd, &rect);
		DrawText(hdc, L"Test tekst", -1, &rect, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);

		EndPaint(m_hwnd, &ps);
	}*/
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(L"Drag'n'Force Serial Terminal", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;
	case WM_SIZE:
		Resize();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}