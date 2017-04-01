//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider* 			m_AttenuationConstantSlider;
	Fl_Slider* 			m_AttenuationLinearSlider;
	Fl_Slider* 			m_AttenuationQuadraticSlider;
	Fl_Slider*			m_AmbientLightSlider;
	Fl_Slider* 			m_IntensitySlider;
	Fl_Slider* 			m_DistanceSlider;
	Fl_Slider* 			m_AntialiasingSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Light_Button*	m_fresnelSwitch;
	Fl_Light_Button* 	m_jitteringSwitch;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double 		getAttenuationConstant();
	double 		getAttenuationLinear();
	double 		getAttenuationQuadratic();
	double 		getAmbientLight();
	int 		getIntensity();
	double		getDistance();
	int 		getAntialiasingSize();
	bool 		isEnableFresnel();
	bool 		isEnableJittering();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double 		m_dAttenuationConstant;
	double 		m_dAttenuationLinear;
	double		m_dAttenuationQuadratic;
	double		m_dAmbientLight;
	int 		m_nIntensity;
	double 		m_nDistance;
	int 		m_nAntialiasingSize;
	bool 		m_bIsEnableFresnel;
	bool 		m_bIsEnableJittering;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void	cb_load_background_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_attenuationConstantSlides(Fl_Widget* o, void* v);
	static void cb_attenuationLinearSlides(Fl_Widget* o, void* v);
	static void cb_attenuationQuadraticSlides(Fl_Widget* o, void* v);
	static void cb_ambientLightSlides(Fl_Widget* o, void* v);
	static void cb_intensitySlides(Fl_Widget* o, void* v);
	static void cb_distanceSlides(Fl_Widget* o, void* v);
	static void cb_antialiasingSlides(Fl_Widget* o, void* v);
	static void cb_fresnelSwitch(Fl_Widget* o, void* v);
	static void cb_jitteringSwitch(Fl_Widget* o, void* v);


	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
