#define GLEW_STATIC
#include "OGLES/GL/glew.h"

#include "NstOpenGLES.hpp"
#include "language/resource.h"
#include "NstIoLog.hpp"
#include "NstApplicationException.hpp"
#include "NstIoScreen.hpp"

#include <functional>
#include <algorithm>

#if NST_MSVC
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#endif

static void __Log(const char *fmt, ...) {
	static char buf[512];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf_s(buf, sizeof(buf), sizeof(buf), fmt, ap);
	va_end(ap);

	::OutputDebugStringA(buf);
	::OutputDebugStringA("\n");
}

#define LOG(__fmt__, ...) __Log(__fmt__, ##__VA_ARGS__)

#define CHECK_GL_ERROR_DEBUG() \
	do { \
		GLenum __error = glGetError(); \
		if (__error != GL_NO_ERROR) { \
			__Log("OpenGL error 0x%04X in %s %s %d.\n%s", \
				__error, __FILE__, __FUNCTION__, __LINE__, (const char *)glewGetErrorString(__error)); \
		} \
	} while (0)

//GLenum bufferFormat = GL_UNSIGNED_SHORT_4_4_4_4;
//GLenum bufferFormat = GL_UNSIGNED_INT_8_8_8_8;

namespace Nestopia {
	namespace OpenGLES {

		static bool GlewDynamicBinding() {
			const char *gl_extensions = (const char *)glGetString(GL_EXTENSIONS);

			// If the current opengl driver doesn't have framebuffers methods, check if an extension exists
			if (glGenFramebuffers == NULL) {
				__Log("OpenGL: glGenFramebuffers is NULL, try to detect an extension");
				if (strstr(gl_extensions, "ARB_framebuffer_object")) {
					__Log("OpenGL: ARB_framebuffer_object is supported");

					glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
					glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
					glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
					glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
					glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
					glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
					glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
					glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
					glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
					glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
					glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
					glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
					glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
					glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
					glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
					glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
					glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
				}
				else {
					if (strstr(gl_extensions, "EXT_framebuffer_object")) {
						__Log("OpenGL: EXT_framebuffer_object is supported");
						glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbufferEXT");
						glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbufferEXT");
						glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
						glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffersEXT");
						glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorageEXT");
						glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
						glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebufferEXT");
						glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebufferEXT");
						glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
						glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffersEXT");
						glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
						glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
						glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
						glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
						glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
						glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
						glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmapEXT");
					}
					else {
						__Log("OpenGL: No framebuffers extension is supported");
						__Log("OpenGL: Any call to Fbo will crash!");
						return false;
					}
				}
			}
			return true;
		}


		//static uint FormatToBpp(const GLenum format) {
		//	switch (format) {
		//	case GL_UNSIGNED_BYTE_3_3_2:
		//	case GL_UNSIGNED_BYTE_2_3_3_REV: return 8;
		//	case GL_UNSIGNED_SHORT_5_6_5:
		//	case GL_UNSIGNED_SHORT_5_6_5_REV:
		//	case GL_UNSIGNED_SHORT_4_4_4_4:
		//	case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		//	case GL_UNSIGNED_SHORT_5_5_5_1:
		//	case GL_UNSIGNED_SHORT_1_5_5_5_REV: return 16;
		//	case GL_UNSIGNED_INT_8_8_8_8:
		//	case GL_UNSIGNED_INT_8_8_8_8_REV:
		//	case GL_UNSIGNED_INT_10_10_10_2:
		//	case GL_UNSIGNED_INT_2_10_10_10_REV: return 32;
		//	}
		//	return 0;
		//}

		//static void FormatToMask(const GLenum format, ulong& r, ulong& g, ulong& b) {
		//	switch (format) {
		//	case GL_UNSIGNED_BYTE_3_3_2:        r = 0x00E0;     g = 0x001C;     b = 0x0003;     break;
		//	//case GL_UNSIGNED_BYTE_2_3_3_REV:
		//	case GL_UNSIGNED_SHORT_5_6_5:       r = 0xF800;     g = 0x07E0;     b = 0x001F;     break;
		//	//case GL_UNSIGNED_SHORT_5_6_5_REV:
		//	case GL_UNSIGNED_SHORT_4_4_4_4:	    r = 0x0F00;     g = 0x00F0;     b = 0x000F;     break;
		//	//case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		//	case GL_UNSIGNED_SHORT_5_5_5_1:     r = 0x7C00;     g = 0x03E0;     b = 0x001F;     break;
		//	//case GL_UNSIGNED_SHORT_1_5_5_5_REV:
		//	case GL_UNSIGNED_INT_8_8_8_8:       r = 0x00FF0000; g = 0x0000FF00; b = 0x000000FF; break;
		//	case GL_UNSIGNED_INT_8_8_8_8_REV:   r = 0x000000FF; g = 0x0000FF00; b = 0x00FF0000; break;
		//	case GL_UNSIGNED_INT_10_10_10_2:    r = 0x3FF00000; g = 0x000FFC00; b = 0x000003FF; break;
		//	case GL_UNSIGNED_INT_2_10_10_10_REV:r = 0x000003FF; g = 0x000FFC00; b = 0x3FF00000; break;
		//	default: r = 0; g = 0; b = 0; break;
		//	}
		//}

		GLView::GLView(HWND hWnd)
		:
		adapters(EnumerateAdapters()),
		windowed(true),
		dialogBoxMode(false),
		hDeviceWindow(hWnd),
		hDC(NULL),
		hRC(NULL),
		ordinal(0),
		pitch(0),
		texture(0),
		lastResult(S_OK) {
			hDC = ::GetDC(hDeviceWindow);
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),  // size
				1,                          // version
				PFD_SUPPORT_OPENGL |        // OpenGL window
				PFD_DRAW_TO_WINDOW |        // render to window
				PFD_DOUBLEBUFFER,           // support double-buffering
				PFD_TYPE_RGBA,              // color type
				32,                         // preferred color depth
				0, 0, 0, 0, 0, 0,           // color bits (ignored)
				0,                          // no alpha buffer
				0,                          // alpha bits (ignored)
				0,                          // no accumulation buffer
				0, 0, 0, 0,                 // accum bits (ignored)
				24,                         // depth buffer
				8,                          // no stencil buffer
				0,                          // no auxiliary buffers
				PFD_MAIN_PLANE,             // main layer
				0,                          // reserved
				0, 0, 0,                    // no layer, visible, damage masks
			};

			int pixelFormat = ::ChoosePixelFormat(hDC, &pfd);
			::SetPixelFormat(hDC, pixelFormat, &pfd);

			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);

			GLenum initResult = glewInit();
			if (initResult != GLEW_OK) {
				LOG((const char *)glewGetErrorString(initResult));
				throw Application::Exception(L"OpenGL error when init");
			}

			if (!GlewDynamicBinding()) {
				throw Application::Exception(L"No OpenGL framebuffer support. Please upgrade the driver of your video card.");
			}

			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		}

		GLView::~GLView() {
			if (hDC != NULL && hRC != NULL) {
				wglMakeCurrent(hDC, NULL);
				wglDeleteContext(hRC);
			}
		}

		void GLView::RenderScreen(uint state) {
			if (SUCCEEDED(lastResult)) {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				if (state & RENDER_PICTURE) {
					RenderPicture();
				}

				lastResult = S_OK;
			}
		}

		void GLView::RenderPicture() {
			const GLsizei width = screenSize.x, height = screenSize.y;
			if (width == 0 || height == 0) {
				return;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, &pixels[0]);

			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			glBindTexture(GL_TEXTURE_2D, texture);

			static const GLfloat coordinates[] = {
				0.0f, 1.0f,
				1.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f
			};

			const GLfloat width0 = (GLfloat)pictureRect.right;
			const GLfloat height0 = (GLfloat)pictureRect.bottom;

			GLfloat vertices[] = {
				0.0f, 0.0f,
				width0, 0.0f,
				0.0f, height0,
				width0, height0
			};

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, vertices);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, coordinates);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			CHECK_GL_ERROR_DEBUG();
		}

		void GLView::SelectAdapter(const Adapters::const_iterator adapter) {
			if (ordinal != adapter->ordinal) {
			}
		}

		bool GLView::CanSwitchFullscreen(const Adapter::Modes::const_iterator mode) const {
			return windowed;
		}

		bool GLView::SwitchFullscreen(const Adapter::Modes::const_iterator mode) {
			if (CanSwitchFullscreen(mode)) {
				windowed = false;
				//presentation.FullScreen_RefreshRateInHz = GetDesiredPresentationRate(mode);

				if (FAILED(Reset())) {
					throw Application::Exception(L"Couldn't switch display mode!");
				}

				//fonts.Create(*this);

				lastResult = S_OK;
				return true;
			}

			return false;
		}

		bool GLView::SwitchWindowed() {
			if (!windowed) {
				windowed = true;
				if (dialogBoxMode) {
					dialogBoxMode = false;
				}

				if (FAILED(Reset())) {
					throw Application::Exception(L"Couldn't switch display mode!");
				}

				lastResult = S_OK;
				return true;
			}
			return false;
		}

		void GLView::EnableDialogBoxMode(const bool enable) {
			if (!windowed && dialogBoxMode != enable) {
				lastResult = ToggleDialogBoxMode();
			}
		}

		LRESULT GLView::ToggleDialogBoxMode() {
			NST_ASSERT(!windowed);

			if (dialogBoxMode) {
				dialogBoxMode = false;
			}
			else {
				dialogBoxMode = true;
			}

			return DeviceReset();
		}

		bool GLView::Reset() {
			if (FAILED(lastResult) && lastResult != INVALID_RECT) {
				lastResult = DeviceRepair(lastResult);
			}

			return SUCCEEDED(lastResult);
		}

		void GLView::UpdateWindowView() {
			const Point::Picture picture(hDeviceWindow);

			if (picture.x > 0 && picture.y > 0) {
				const Point::Client client(hDeviceWindow);
				NST_ASSERT(client.x >= picture.x && client.y >= picture.y);

				if (lastResult == INVALID_RECT) {
					lastResult = DeviceResetWindowClient(client, lastResult);
				}
			}
			else {
				lastResult = INVALID_RECT;
			}
		}

		void GLView::UpdateWindowView(
			const Point& screen,
			const Rect& clipping,
			const uint scanlines,
			const int screenCurvature,
			const Adapter::Filter filter,
			const bool useVidMem) {
			const Point::Picture picture(hDeviceWindow);

			if (picture.x > 0 && picture.y > 0) {
				UpdateGL(picture, screen, clipping, scanlines, screenCurvature, filter, useVidMem);

				const Point::Client client(hDeviceWindow);
				NST_ASSERT(client.x >= picture.x && client.y >= picture.y);

				if (lastResult == INVALID_RECT) {
					lastResult = DeviceResetWindowClient(client, lastResult);
				}
			}
			else {
				lastResult = INVALID_RECT;
			}
		}

		void GLView::UpdateFullscreenView(
			const Rect& picture,
			const Point& screen,
			const Rect& clipping,
			const uint scanlines,
			const int screenCurvature,
			const Adapter::Filter filter,
			const bool useVidMem) {
			NST_ASSERT(picture.Width() && picture.Height());

			UpdateGL(picture, screen, clipping, scanlines, screenCurvature, filter, useVidMem);
		}

		void GLView::UpdateFrameRate(const uint frameRate, const bool vsync, const bool tripleBuffering) {
		}

		void GLView::UpdateGL(
			const Rect& picture,
			const Point& screen,
			const Rect& clipping,
			const uint scanlines,
			const int screenCurvature,
			const Adapter::Filter filter,
			const bool useVidMem) {
			NST_ASSERT(picture.Width() > 0 && picture.Height() > 0 && clipping.Width() > 0 && clipping.Height() > 0);

			pictureRect = picture;
			screenSize = screen;

			glViewport(0, 0, pictureRect.right, pictureRect.bottom);
			glMatrixMode(GL_PROJECTION);

			glLoadIdentity();
			glOrtho(0.0f, pictureRect.right, 0.0f, pictureRect.bottom, -1.0f, 1.0f);

			if (texture == 0) {
				glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				const int width = screenSize.x;
				const int height = screenSize.y;
				NST_ASSERT(width != 0 && height != 0);

				const size_t pixelSize = 4;
				pitch = width * 4;
				size_t totalSize = pitch * height;
				pixels.resize(totalSize);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, &pixels[0]);
			}
		}

		HRESULT GLView::DeviceResetWindowClient(const Point& client, HRESULT hResult) {
			NST_ASSERT(windowed && client.x > 0 && client.y > 0);

			if (SUCCEEDED(hResult) || hResult == INVALID_RECT) {
				hResult = DeviceReset();
			}

			return hResult;
		}

		HRESULT GLView::DeviceReset() {
			return S_OK;
		}

		HRESULT GLView::DeviceRepair(const HRESULT lastError) {
			return S_OK;
		}

		GLView::ScreenShotResult GLView::SaveScreenShot(wcstring const file, const uint ext) const {
			NST_ASSERT(file && *file);

			return SCREENSHOT_ERROR;
		}

		GLView::Mode::Mode(uint w, uint h, uint b)
		: width(w), height(h), bpp(b) {}

		bool GLView::Mode::operator < (const Mode& mode) const {
			if ( width  < mode.width  ) return true;
			if ( width  > mode.width  ) return false;
			if ( height < mode.height ) return true;
			if ( height > mode.height ) return false;
			if ( bpp    < mode.bpp    ) return true;
			if ( bpp    > mode.bpp    ) return false;

			return false;
		}

		bool GLView::Windowed() const {
			return windowed;
		}

		bool GLView::ThrottleRequired(uint speed) const {
			return false;
		}

		GLView::Adapters GLView::EnumerateAdapters() {
			Adapters adapters;
			adapters.push_back(Adapter());
			Adapter& adapter = adapters.back();
			//adapter.guid = identifier.DeviceIdentifier;
			adapter.name = "Unknown";
			adapter.ordinal = 0;
			adapter.deviceType = Adapter::DEVICE_HEL;
			adapter.maxScreenSize = Point(::GetSystemMetrics(SM_CXFULLSCREEN), ::GetSystemMetrics(SM_CYFULLSCREEN));
			adapter.videoMemScreen = false;
			adapter.canDoScanlineEffect = false;
			adapter.filters = 0;// adapter.filters |= Adapter::FILTER_BILINEAR;
			adapter.modern = true;//!!glewIsSupported("GL_VERSION_2_0")
			const_cast<Mode::Rates &>(adapter.modes.insert(Mode(Mode::MIN_WIDTH, Mode::MIN_HEIGHT, 16)).first->rates).insert(Mode::MAX_RATE);
			const_cast<Mode::Rates &>(adapter.modes.insert(Mode(Mode::MIN_WIDTH, Mode::MIN_HEIGHT, 32)).first->rates).insert(Mode::MAX_RATE);

			if (adapters.empty()) {
				throw Application::Exception(L"Found no valid display adapter!");
			}

			return adapters;
		}

		uint GLView::GetBitsPerPixel() const {
			//return FormatToBpp(bufferFormat);
			return 32;
		}

		void GLView::GetBitMask(ulong& r, ulong& g, ulong& b) const {
			//FormatToMask(bufferFormat, r, g, b);

			//r = 0x00FF0000; g = 0x0000FF00; b = 0x000000FF;  //	case GL_UNSIGNED_INT_8_8_8_8
			r = 0x000000FF; g = 0x0000FF00; b = 0x00FF0000;  //	case GL_UNSIGNED_INT_8_8_8_8_REV;
		}

		bool GLView::LockScreen(void*& data, long& pitch) {
			if (!pixels.empty()) {
				data = &pixels[0];
				pitch = this->pitch;
				return true;
			}
			return false;
		}

		void GLView::UnlockScreen() const {
		}

		bool GLView::ClearScreen() {
			return true;
		}

		bool GLView::PresentScreen() {
			if (SUCCEEDED(lastResult)) {
				lastResult = ::SwapBuffers(hDC) ? S_OK : E_FAIL;
				return SUCCEEDED(lastResult);
			}
			else {
				return lastResult == INVALID_RECT;
			}
		}

		void GLView::DrawFps(const GenericString& string) {

		}

		void GLView::ClearFps() {

		}

		void GLView::DrawMsg(const GenericString& string) {

		}

		void GLView::ClearMsg() {

		}

		void GLView::DrawNfo(const GenericString& string) {

		}

		void GLView::ClearNfo() {

		}

		void GLView::EnableAutoFrequency(bool enable) {

		}

		uint GLView::GetMaxMessageLength() const {
			return 0;
		}

		const GLView::Point GLView::GetFullscreenDisplayMode() const {
			return Point(::GetSystemMetrics(SM_CXFULLSCREEN), ::GetSystemMetrics(SM_CYFULLSCREEN));
		}

		bool GLView::SmoothFrameRate() const {
			return false;
		}
	}
}
