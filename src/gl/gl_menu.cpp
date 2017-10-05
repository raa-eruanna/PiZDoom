

#include "m_menu.h"
#include "gl/gl_intern.h"


extern value_t YesNo[2];
extern value_t NoYes[2];
extern value_t OnOff[2];
extern bool gl_disabled;

void StartGLLightMenu (void);
void StartDisableGL();
void ReturnToMainMenu();

CUSTOM_CVAR(Bool, gl_nogl, true, CVAR_GLOBALCONFIG|CVAR_ARCHIVE|CVAR_NOINITCALL)
{
	Printf("This won't take effect until ZDoom is restarted.\n");
}

EXTERN_CVAR (Bool, vid_vsync)
EXTERN_CVAR(Int, gl_spriteclip)
EXTERN_CVAR(Int, gl_lightmode)
EXTERN_CVAR(Bool, gl_blendcolormaps)
EXTERN_CVAR(Bool, gl_texture_usehires)
EXTERN_CVAR(Bool, gl_precache)
EXTERN_CVAR(Bool, gl_render_precise)
EXTERN_CVAR(Bool, gl_sprite_blend)
EXTERN_CVAR(Bool, gl_fakecontrast)
EXTERN_CVAR (Bool, gl_lights_additive)
EXTERN_CVAR(Bool, gl_warp_shader)
EXTERN_CVAR (Float, gl_light_ambient)
EXTERN_CVAR(Int, gl_billboard_mode)
EXTERN_CVAR(Int, gl_texture_hqresize)
EXTERN_CVAR(Flag, gl_texture_hqresize_textures)
EXTERN_CVAR(Flag, gl_texture_hqresize_sprites)
EXTERN_CVAR(Flag, gl_texture_hqresize_fonts)

static value_t SpriteclipModes[]=
{
	{ 0.0, "Never" },
	{ 1.0, "Smart" },
	{ 2.0, "Always" },
};

static value_t FilterModes[] =
{
	{ 0.0, "None" },
	{ 1.0, "None (mipmapped)" },
	{ 2.0, "Linear" },
	{ 3.0, "Bilinear" },
	{ 4.0, "Trilinear" },
};

static value_t TextureFormats[] =
{
	{ 0.0, "RGBA8" },
	{ 1.0, "RGB5_A1" },
	{ 2.0, "RGBA4" },
	{ 3.0, "RGBA2" },
	// [BB] Added modes for texture compression.
	{ 4.0, "COMPR_RGBA" },
	{ 5.0, "S3TC_DXT1" },
	{ 6.0, "S3TC_DXT3" },
	{ 7.0, "S3TC_DXT5" },
};

static value_t Anisotropy[] =
{
	{ 1.0, "Off" },
	{ 2.0, "2x" },
	{ 4.0, "4x" },
	{ 8.0, "8x" },
	{ 16.0, "16x" },
};

static value_t Colormaps[] =
{
	{ 0.0, "Use as palette" },
	{ 1.0, "Blend" },
};

static value_t LightingModes[] =
{
	{ 0.0, "Standard" },
	{ 1.0, "Bright" },
	{ 3.0, "Doom" },
	{ 4.0, "Legacy" },
};

static value_t Precision[] =
{
	{ 0.0, "Speed" },
	{ 1.0, "Quality" },
};


static value_t Hz[] =
{
	{ 0.0, "Optimal" },
	{ 60.0, "60" },
	{ 70.0, "70" },
	{ 72.0, "72" },
	{ 75.0, "75" },
	{ 85.0, "85" },
	{ 100.0, "100" }
};

static value_t BillboardModes[] =
{
	{ 0.0, "Y Axis" },
	{ 1.0, "X/Y Axis" },
};

static value_t HqResizeModes[] =
{
   { 0.0, "Off" },
   { 1.0, "Scale2x" },
   { 2.0, "Scale3x" },
   { 3.0, "Scale4x" },
   { 4.0, "hq2x" },
   { 5.0, "hq3x" },
   { 6.0, "hq4x" },
   { 7.0, "xBRZ 2x" },
   { 8.0, "xBRZ 3x" },
   { 9.0, "xBRZ 4x" },
   { 10.0, "xBRZ_old 2x" },
   { 11.0, "xBRZ_old 3x" },
   { 12.0, "xBRZ_old 4x" },
};
 
static value_t HqResizeTargets[] =
{
   { 0.0, "Everything" },
   { 1.0, "Sprites/fonts" },
};

menuitem_t OpenGLItems[] = {
	{ more,     "Dynamic Light Options", {NULL}, {0.0}, {0.0},	{0.0}, {(value_t *)StartGLLightMenu} },
	{ more,     "Disable GL system",	 {NULL}, {0.0}, {0.0},	{0.0}, {(value_t *)StartDisableGL} },
	{ redtext,	" ",						{NULL},							{0.0}, {0.0}, {0.0}, {NULL} },
	{ discrete, "Vertical Sync",			{&vid_vsync},					{2.0}, {0.0}, {0.0}, {OnOff} },
//	{ discrete, "Refresh rate",				{&gl_vid_refreshHz},			{7.0}, {0.0}, {0.0}, {Hz} },
//	{ more,		"Apply Refresh rate setting",{NULL+},						{7.0}, {0.0}, {0.0}, {(value_t *)ApplyRefresh} },
	{ discrete, "Rendering quality",		{&gl_render_precise},			{2.0}, {0.0}, {0.0}, {Precision} },
	{ discrete, "Environment map on mirrors",{&gl_mirror_envmap},			{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Enhanced night vision mode",{&gl_enhanced_lightamp},		{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Sector light mode",		{&gl_lightmode},				{4.0}, {0.0}, {0.0}, {LightingModes} },
	{ discrete, "Sprite billboard",			{&gl_billboard_mode},			{2.0}, {0.0}, {0.0}, {BillboardModes} },
	{ discrete, "Adjust sprite clipping",	{&gl_spriteclip},				{3.0}, {0.0}, {0.0}, {SpriteclipModes} },
	{ discrete, "Smooth sprite edges",		{&gl_sprite_blend},				{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Shaders for texture warp",	{&gl_warp_shader},				{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Depth Fog",				{&gl_depthfog},					{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Fake contrast",			{&gl_fakecontrast},				{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Boom colormap handling",	{&gl_blendcolormaps},			{2.0}, {0.0}, {0.0}, {Colormaps} },
	{ slider,	"Ambient light level",		{&gl_light_ambient},			{0.0}, {255.0}, {5.0}, {NULL} },
	{ redtext,	" ",						{NULL},							{0.0}, {0.0}, {0.0}, {NULL} },
	{ discrete, "Textures enabled",			{&gl_texture},					{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "Texture Filter mode",		{&gl_texture_filter},			{5.0}, {0.0}, {0.0}, {FilterModes} },
	{ discrete, "Anisotropic filter",		{&gl_texture_filter_anisotropic},{5.0},{0.0}, {0.0}, {Anisotropy} },
	{ discrete, "Texture Format",			{&gl_texture_format},			{8.0}, {0.0}, {0.0}, {TextureFormats} },
	{ discrete, "Enable hires textures",	{&gl_texture_usehires},			{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "High Quality Resize mode",	{&gl_texture_hqresize},			{13.0}, {0.0}, {0.0}, {HqResizeModes} },
	{ discrete, "Resize textures",			{&gl_texture_hqresize_textures},{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Resize sprites",			{&gl_texture_hqresize_sprites},	{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Resize fonts",				{&gl_texture_hqresize_fonts},	{2.0}, {0.0}, {0.0}, {OnOff} },
	{ discrete, "Precache GL textures",		{&gl_precache},					{2.0}, {0.0}, {0.0}, {YesNo} },
};

menuitem_t GLLightItems[] = {
	{ discrete, "Dynamic Lights enabled",	{&gl_lights},			{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "Enable light definitions",	{&gl_attachedlights},	{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "Clip lights",				{&gl_lights_checkside},	{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "Lights affect sprites",	{&gl_light_sprites},	{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "Lights affect particles",	{&gl_light_particles},	{2.0}, {0.0}, {0.0}, {YesNo} },
	{ discrete, "Force additive lighting",	{&gl_lights_additive},	{2.0}, {0.0}, {0.0}, {YesNo} },
	{ slider,	"Light intensity",			{&gl_lights_intensity}, {0.0}, {1.0}, {0.1f}, {NULL} },
	{ slider,	"Light size",				{&gl_lights_size},		{0.0}, {2.0}, {0.1f}, {NULL} },
};

menuitem_t OpenGLDisabled[] = {
	{ redtext,	"This won't take effect",			{NULL},	{0.0}, {0.0}, {0.0}, {NULL} },
	{ redtext,	"until ZDoom is restarted.",		{NULL},	{0.0}, {0.0}, {0.0}, {NULL} },
	{ more,     "",									{NULL}, {0.0}, {0.0},	{0.0}, {(value_t *)ReturnToMainMenu} },
};

menu_t OpenGLMenu = {
   "OPENGL OPTIONS",
   0,
   sizeof(OpenGLItems)/sizeof(OpenGLItems[0]),
   0,
   OpenGLItems,
   0,
};

menu_t OpenGLMessage = {
   "",
   2,
   sizeof(OpenGLDisabled)/sizeof(OpenGLDisabled[0]),
   0,
   OpenGLDisabled,
   0,
};

menu_t GLLightMenu = {
   "LIGHT OPTIONS",
   0,
   sizeof(GLLightItems)/sizeof(GLLightItems[0]),
   0,
   GLLightItems,
   0,
};

void StartDisableGL()
{
	M_SwitchMenu(&OpenGLMessage);
	gl_nogl=true;
}

void ReturnToMainMenu()
{
	M_StartControlPanel(false);
}

void StartGLMenu (void)
{
	if (!gl_disabled)
	{
		M_SwitchMenu(&OpenGLMenu);
	}
	else
	{
		M_SwitchMenu(&OpenGLMessage);
		gl_nogl=false;
	}
}

void StartGLLightMenu (void)
{
	M_SwitchMenu(&GLLightMenu);
}


