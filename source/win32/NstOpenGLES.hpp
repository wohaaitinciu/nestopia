#ifndef NST_OPENGL_GLVIEW_H
#define NST_OPENGL_GLVIEW_H

#pragma once

#include <set>
#include <vector>
#include "NstWindowRect.hpp"
#include "NstSystemGuid.hpp"

//#ifdef NST_DEBUG
//#define GL_AMD_debug_output 1
//#define GL_ARB_debug_output 1
//#else
//#define GL_AMD_debug_output 0
//#define GL_ARB_debug_output 0
//#endif

#include "OGLES/GL/glew.h"

namespace Nestopia {
	namespace OpenGLES {

		class GLView {
		public:
			explicit GLView(HWND);
			~GLView();

			typedef Window::Rect Rect;
			typedef Window::Point Point;

			enum { MAX_SCANLINES = 100 };

			struct Mode {
				typedef std::set<uchar> Rates;

				explicit Mode(uint w = 0, uint h = 0, uint b = 0);

				enum {
					MIN_WIDTH = 256,
					MIN_HEIGHT = 240,
					DEFAULT_RATE = 60,
					MAX_RATE = 120
				};

				bool operator == (const Mode& mode) const {
					return width == mode.width && height == mode.height && bpp == mode.bpp;
				}
				bool operator <  (const Mode&) const;

				uint width, height, bpp;
				Rates rates;

				bool operator!=(const Mode& mode) const { return !(*this == mode); }

				Point Size() const { return Point(width, height); }
			};

			struct Adapter {
				bool operator == (const Adapter& a)  const { return guid == a.guid; }
				bool operator <  (const Adapter& a)  const { return guid < a.guid; }
				bool operator == (const System::Guid& g) const { return guid == g; }

				System::Guid guid;
				HeapString name;

				typedef std::set<Mode> Modes;

				enum DeviceType {
					DEVICE_HAL,
					DEVICE_HEL
				};

				enum Filter {
					FILTER_NONE,
					FILTER_BILINEAR
				};

				uint ordinal;
				DeviceType deviceType;
				Point maxScreenSize;
				bool videoMemScreen;
				bool canDoScanlineEffect;
				bool modern;
				uint filters;
				Modes modes;
			};

			typedef std::vector<Adapter> Adapters;

			enum {
				RENDER_PICTURE = 0x01,
				RENDER_FPS     = 0x04,
				RENDER_MSG     = 0x08,
				RENDER_NFO     = 0x10
			};

			void SelectAdapter(const Adapters::const_iterator);
			void RenderScreen(uint);
			bool CanSwitchFullscreen(const Adapter::Modes::const_iterator) const;
			bool SwitchFullscreen(const Adapter::Modes::const_iterator);
			bool SwitchWindowed();
			void UpdateWindowView();
			void UpdateWindowView(const Point& screen,
				const Rect& clipping,
				const uint scanlines,
				int screenCurvature,
				Adapter::Filter filter,
				bool useVidMem);
			void UpdateFullscreenView(const Rect& picture,
				const Point& screen,
				const Rect& clipping,
				uint scanlines,
				int screenCurvature,
				Adapter::Filter filter,
				bool useVidMem);
			void UpdateFrameRate(uint frameRate, bool vsync, bool tripleBuffering);
			void EnableDialogBoxMode(bool);
			bool Reset();

			enum ScreenShotResult {
				SCREENSHOT_OK,
				SCREENSHOT_UNSUPPORTED,
				SCREENSHOT_ERROR
			};

			ScreenShotResult SaveScreenShot(wcstring,uint) const;

		private:
			const Adapters adapters;

			Rect pictureRect;
			Point screenSize;

			bool windowed;
			bool dialogBoxMode;

			HWND hDeviceWindow;
			HDC hDC;
			HGLRC hRC;

			uchar ordinal;

			long pitch;
			std::vector<GLubyte> pixels;
			GLuint texture;

			enum {
				INVALID_RECT = MAKE_HRESULT(SEVERITY_ERROR, 0x123, 2),
			};
			HRESULT lastResult;

		private:
			static Adapters EnumerateAdapters();

			HRESULT DeviceResetWindowClient(const Point& client, HRESULT hResult);
			HRESULT DeviceReset();
			HRESULT DeviceRepair(const HRESULT lastError);

			LRESULT ToggleDialogBoxMode();

			void RenderPicture();
			void UpdateGL(const Rect& picture, const Point& screen, const Rect& clipping, uint scanlines, int screenCurvature, Adapter::Filter filter, bool useVidMem);

		public:

			bool ValidScreen() const { return SUCCEEDED(lastResult); }
			bool Windowed() const;
			bool ThrottleRequired(uint speed) const;

			const Adapters& GetAdapters() const { return adapters; }

			const Adapter& GetAdapter() const {
				NST_ASSERT(ordinal < adapters.size());
				return adapters[ordinal];
			}

			uint GetBitsPerPixel() const;
			void GetBitMask(ulong& r, ulong& g, ulong& b) const;
			const Rect& GetScreenRect() const { return pictureRect; }

			bool LockScreen(void*& data, long& pitch);
			void UnlockScreen() const;
			bool ClearScreen();
			bool PresentScreen();
			void DrawFps(const GenericString& string);
			void ClearFps();
			void DrawMsg(const GenericString& string);
			void ClearMsg();
			void DrawNfo(const GenericString& string);
			void ClearNfo();
			void EnableAutoFrequency(bool enable);
			uint GetMaxMessageLength() const;
			const Point GetFullscreenDisplayMode() const;
			bool ModernGPU() const { return adapters[ordinal].modern; }
			bool SmoothFrameRate() const;
		};
	}
}

#endif
