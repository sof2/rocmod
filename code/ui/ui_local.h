// Copyright (C) 2001-2002 Raven Software.
//
#ifndef __UI_LOCAL_H__
#define __UI_LOCAL_H__

#ifndef	__Q_SHARED_H
	#include "../game/q_shared.h"
#endif

#include "../cgame/tr_types.h"
#include "ui_public.h"
#include "keycodes.h"
#include "../game/bg_public.h"
#include "ui_shared.h"

// global display context

extern vmCvar_t	ui_arenasFile;
extern vmCvar_t	ui_botsFile;

extern vmCvar_t	ui_browserMaster;
extern vmCvar_t	ui_browserGameType;
extern vmCvar_t	ui_browserSortKey;
extern vmCvar_t	ui_browserShowFull;
extern vmCvar_t	ui_browserShowEmpty;

extern vmCvar_t	ui_drawCrosshair;
extern vmCvar_t	ui_drawCrosshairNames;

extern vmCvar_t	ui_server1;
extern vmCvar_t	ui_server2;
extern vmCvar_t	ui_server3;
extern vmCvar_t	ui_server4;
extern vmCvar_t	ui_server5;
extern vmCvar_t	ui_server6;
extern vmCvar_t	ui_server7;
extern vmCvar_t	ui_server8;
extern vmCvar_t	ui_server9;
extern vmCvar_t	ui_server10;
extern vmCvar_t	ui_server11;
extern vmCvar_t	ui_server12;
extern vmCvar_t	ui_server13;
extern vmCvar_t	ui_server14;
extern vmCvar_t	ui_server15;
extern vmCvar_t	ui_server16;

extern vmCvar_t	ui_cdkey;
extern vmCvar_t	ui_cdkeychecked;
extern vmCvar_t	ui_invalidversion;

extern vmCvar_t	ui_scoreLimit;
extern vmCvar_t	ui_gameType;
extern vmCvar_t	ui_netGameType;
extern vmCvar_t	ui_actualNetGameType;
extern vmCvar_t	ui_joinGameType;
extern vmCvar_t	ui_netSource;
extern vmCvar_t	ui_serverFilterType;
extern vmCvar_t	ui_dedicated;
extern vmCvar_t	ui_menuFiles;
extern vmCvar_t	ui_currentMap;
extern vmCvar_t	ui_currentNetMap;
extern vmCvar_t	ui_mapIndex;
extern vmCvar_t	ui_selectedPlayer;
extern vmCvar_t	ui_selectedPlayerName;
extern vmCvar_t	ui_lastServerRefresh_0;
extern vmCvar_t	ui_lastServerRefresh_1;
extern vmCvar_t	ui_lastServerRefresh_2;
extern vmCvar_t	ui_lastServerRefresh_3;
extern vmCvar_t ui_serverStatusTimeOut;
extern vmCvar_t	ui_botteam;

extern vmCvar_t	ui_info_redteam;
extern vmCvar_t	ui_info_blueteam;
extern vmCvar_t ui_info_redcount;
extern vmCvar_t ui_info_bluecount;
extern vmCvar_t ui_info_redscore;
extern vmCvar_t	ui_info_bluescore;
extern vmCvar_t	ui_info_timer;
extern vmCvar_t	ui_info_objectives;
extern vmCvar_t	ui_info_gametype;
extern vmCvar_t	ui_info_team;
extern vmCvar_t	ui_joinserver;

extern vmCvar_t	ui_allowparental;

extern vmCvar_t	ui_noNetCheck;

typedef void		*TGhoul2;

#define G2SURFACEFLAG_ISBOLT		0x00000001
#define G2SURFACEFLAG_OFF			0x00000002	// saves strcmp()ing for "_off" in surface names
#define G2SURFACEFLAG_SPARE0		0x00000004	// future-expansion fields, saves invalidating models if we add more
#define G2SURFACEFLAG_SPARE1		0x00000008	//   
#define G2SURFACEFLAG_SPARE2		0x00000010	// 
#define G2SURFACEFLAG_SPARE3		0x00000020	// 
#define G2SURFACEFLAG_SPARE4		0x00000040	// 
#define G2SURFACEFLAG_SPARE5		0x00000080	// 
//
#define G2SURFACEFLAG_NODESCENDANTS 0x00000100	// ingame-stuff, never generated by Carcass....
#define G2SURFACEFLAG_GENERATED		0x00000200	//

//
// ui_qmenu.c
//

#define RCOLUMN_OFFSET			( BIGCHAR_WIDTH )
#define LCOLUMN_OFFSET			(-BIGCHAR_WIDTH )

#define SLIDER_RANGE			10
#define	MAX_EDIT_LINE			256

#define MAX_MENUDEPTH			8
#define MAX_MENUITEMS			128

#define MTYPE_NULL				0
#define MTYPE_SLIDER			1	
#define MTYPE_ACTION			2
#define MTYPE_SPINCONTROL		3
#define MTYPE_FIELD				4
#define MTYPE_RADIOBUTTON		5
#define MTYPE_BITMAP			6	
#define MTYPE_TEXT				7
#define MTYPE_SCROLLLIST		8
#define MTYPE_PTEXT				9
#define MTYPE_BTEXT				10

#define QMF_BLINK				0x00000001
#define QMF_SMALLFONT			0x00000002
#define QMF_LEFT_JUSTIFY		0x00000004
#define QMF_CENTER_JUSTIFY		0x00000008
#define QMF_RIGHT_JUSTIFY		0x00000010
#define QMF_NUMBERSONLY			0x00000020	// edit field is only numbers
#define QMF_HIGHLIGHT			0x00000040
#define QMF_HIGHLIGHT_IF_FOCUS	0x00000080	// steady focus
#define QMF_PULSEIFFOCUS		0x00000100	// pulse if focus
#define QMF_HASMOUSEFOCUS		0x00000200
#define QMF_NOONOFFTEXT			0x00000400
#define QMF_MOUSEONLY			0x00000800	// only mouse input allowed
#define QMF_HIDDEN				0x00001000	// skips drawing
#define QMF_GRAYED				0x00002000	// grays and disables
#define QMF_INACTIVE			0x00004000	// disables any input
#define QMF_NODEFAULTINIT		0x00008000	// skip default initialization
#define QMF_OWNERDRAW			0x00010000
#define QMF_PULSE				0x00020000
#define QMF_LOWERCASE			0x00040000	// edit field is all lower case
#define QMF_UPPERCASE			0x00080000	// edit field is all upper case
#define QMF_SILENT				0x00100000

// callback notifications
#define QM_GOTFOCUS				1
#define QM_LOSTFOCUS			2
#define QM_ACTIVATED			3

typedef struct _tag_menuframework
{
	int	cursor;
	int cursor_prev;

	int	nitems;
	void *items[MAX_MENUITEMS];

	void (*draw) (void);
	sfxHandle_t (*key) (int key);

	qboolean	wrapAround;
	qboolean	fullscreen;
	qboolean	showlogo;
} menuframework_s;

typedef struct
{
	int type;
	const char *name;
	int	id;
	int x, y;
	int left;
	int	top;
	int	right;
	int	bottom;
	menuframework_s *parent;
	int menuPosition;
	unsigned flags;

	void (*callback)( void *self, int event );
	void (*statusbar)( void *self );
	void (*ownerdraw)( void *self );
} menucommon_s;

typedef struct {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
	int		maxchars;
} mfield_t;

typedef struct
{
	menucommon_s	generic;
	mfield_t		field;
} menufield_s;

typedef struct 
{
	menucommon_s generic;

	float minvalue;
	float maxvalue;
	float curvalue;

	float range;
} menuslider_s;

typedef struct
{
	menucommon_s generic;

	int	oldvalue;
	int curvalue;
	int	numitems;
	int	top;
		
	const char **itemnames;

	int width;
	int height;
	int	columns;
	int	seperation;
} menulist_s;

typedef struct
{
	menucommon_s generic;
} menuaction_s;

typedef struct
{
	menucommon_s generic;
	int curvalue;
} menuradiobutton_s;

typedef struct
{
	menucommon_s	generic;
	char*			focuspic;	
	char*			errorpic;
	qhandle_t		shader;
	qhandle_t		focusshader;
	int				width;
	int				height;
	float*			focuscolor;
} menubitmap_s;

typedef struct
{
	menucommon_s	generic;
	char*			string;
	int				style;
	float*			color;
} menutext_s;

extern void			Menu_Cache( void );
extern void			Menu_Focus( menucommon_s *m );
extern void			Menu_AddItem( menuframework_s *menu, void *item );
extern void			Menu_AdjustCursor( menuframework_s *menu, int dir );
extern void			Menu_Draw( menuframework_s *menu );
extern void			*Menu_ItemAtCursor( menuframework_s *m );
extern sfxHandle_t	Menu_ActivateItem( menuframework_s *s, menucommon_s* item );
extern void			Menu_SetCursor( menuframework_s *s, int cursor );
extern void			Menu_SetCursorToItem( menuframework_s *m, void* ptr );
extern sfxHandle_t	Menu_DefaultKey( menuframework_s *s, int key );
extern void			Bitmap_Init( menubitmap_s *b );
extern void			Bitmap_Draw( menubitmap_s *b );
extern void			ScrollList_Draw( menulist_s *l );
extern sfxHandle_t	ScrollList_Key( menulist_s *l, int key );
extern sfxHandle_t	menu_in_sound;
extern sfxHandle_t	menu_move_sound;
extern sfxHandle_t	menu_out_sound;
extern sfxHandle_t	menu_buzz_sound;
extern sfxHandle_t	menu_null_sound;
extern sfxHandle_t	weaponChangeSound;
extern vec4_t		menu_text_color;
extern vec4_t		menu_grayed_color;
extern vec4_t		menu_dark_color;
extern vec4_t		menu_highlight_color;
extern vec4_t		menu_red_color;
extern vec4_t		menu_black_color;
extern vec4_t		menu_dim_color;
extern vec4_t		color_black;
extern vec4_t		color_white;
extern vec4_t		color_yellow;
extern vec4_t		color_blue;
extern vec4_t		color_orange;
extern vec4_t		color_red;
extern vec4_t		color_dim;
extern vec4_t		name_color;
extern vec4_t		list_color;
extern vec4_t		listbar_color;
extern vec4_t		text_color_disabled; 
extern vec4_t		text_color_normal;
extern vec4_t		text_color_highlight;

//
// ui_main.c
//
void UI_Report				( void );
void UI_Reload				( void );
void UI_LoadMenus			( const char *menuFile, qboolean reset );
void UI_SetActiveMenu		( uiMenuCommand_t menu );
int	 UI_AdjustTimeByGame	( int time );
void UI_LoadArenas			( void );

void UI_DrawConnectScreen( qboolean overlay );
void UI_DrawLoadingScreen( void ) ;

//
// ui_menu.c
//
extern void MainMenu_Cache( void );
extern void UI_MainMenu(void);
extern void UI_RegisterCvars( void );
extern void UI_UpdateCvars( void );


//
// ui_players.c
//
void	UI_LoadIdentityIcons	( void );
TGhoul2 UI_RegisterIdentity		( const char *identityName, char *animationFile );

//FIXME ripped from cg_local.h
typedef struct {
	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on

	int			frame;
	int			frameTime;			// time when ->frame will be exactly on

	float		backlerp;

	float		yawAngle;
	qboolean	yawing;
	float		pitchAngle;
	qboolean	pitching;

	int			animationNumber;	// may include ANIM_TOGGLEBIT
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact
} lerpFrame_t;

typedef struct {
	// model info
	qhandle_t		playerModel;
	void			*playerG2Model;

	int				legsOldAnim;
	lerpFrame_t		legs;

//	qhandle_t		torsoModel;
//	qhandle_t		torsoSkin;
	lerpFrame_t		torso;

//	qhandle_t		headModel;
//	qhandle_t		headSkin;

	animation_t		animations[MAX_ANIMATIONS];

//	qhandle_t		weaponModel;
//	qhandle_t		barrelModel;
//	qhandle_t		flashModel;
	void			*weaponG2Model;		// ghoul2 models

	vec3_t			flashDlightColor;
	int				muzzleFlashTime;

	// currently in use drawing parms
	vec3_t			viewAngles;
	vec3_t			moveAngles;
	weapon_t		currentWeapon;
	int				legsAnim;
	int				torsoAnim;

	// animation vars
	weapon_t		weapon;
	weapon_t		lastWeapon;
	weapon_t		pendingWeapon;
	int				weaponTimer;
	int				pendingLegsAnim;
	int				torsoAnimationTimer;

	int				pendingTorsoAnim;
	int				legsAnimationTimer;

	qboolean		chat;
	qboolean		newModel;

	qboolean		barrelSpinning;
	float			barrelAngle;
	int				barrelTime;

	int				realWeapon;
} playerInfo_t;

void		UI_DrawPlayer				( float x, float y, float w, float h, playerInfo_t *pi, int time );
void		UI_PlayerInfo_SetIdentity	( playerInfo_t *pi, const char *identityName );
void		UI_PlayerInfo_SetInfo		( playerInfo_t *pi, int legsAnim, int torsoAnim, vec3_t viewAngles, vec3_t moveAngles, weapon_t weaponNum, qboolean chat );
qboolean	UI_RegisterClientIdentity	( playerInfo_t *pi, const char *identityName );

//
// ui_atoms.c
//
// this is only used in the old ui, the new ui has it's own version
typedef struct {
	int					frametime;
	int					realtime;
	int					cursorx;
	int					cursory;
	glconfig_t 	glconfig;
	qboolean		debug;
	qhandle_t		whiteShader;
	qhandle_t		menuBackShader;
	qhandle_t		menuBackShader2;
	qhandle_t		menuBackNoLogoShader;
	qhandle_t		charset;
	qhandle_t		charsetProp;
	qhandle_t		charsetPropGlow;
	qhandle_t		charsetPropB;
	qhandle_t		cursor;
	qhandle_t		rb_on;
	qhandle_t		rb_off;
	float				scale;
	float				bias;
	qboolean		demoversion;
	qboolean		firstdraw;
} uiStatic_t;


// new ui stuff
#define UI_NUMFX 7
#define MAX_HEADS 64
#define MAX_ALIASES 64
#define MAX_HEADNAME  32
#define MAX_MAPS 256		// was 128
#define MAX_SPMAPS 16
#define PLAYERS_PER_TEAM 6	// was 5
#define MAX_PINGREQUESTS		32
#define MAX_ADDRESSLENGTH		64
#define MAX_HOSTNAMELENGTH		22
#define MAX_MAPNAMELENGTH		16
#define MAX_STATUSLENGTH		64
#define MAX_LISTBOXWIDTH		59
#define UI_FONT_THRESHOLD		0.1
#define MAX_DISPLAY_SERVERS		2048
#define MAX_SERVERSTATUS_LINES	128
#define MAX_SERVERSTATUS_TEXT	4096
#define MAX_FOUNDPLAYER_SERVERS	16
#define TEAM_MEMBERS 5
#define MAPS_PER_TIER 3
#define MAX_TIERS 16
#define MAX_MODS 64
#define MAX_DEMOS 256
#define MAX_MOVIES				256
#define	MAX_PLAYERIDENTITIES	200
#define	MAX_TEAMIDENTITIES		6//5 pimpy


typedef struct {
  const char *name;
	const char *imageName;
  qhandle_t headImage;
	const char *base;
	qboolean active;
	int reference;
} characterInfo;

typedef struct {
	const char *name;
	const char *ai;
	const char *action;
} aliasInfo;

typedef struct {
  const char *teamName;
	const char *imageName;
	const char *teamMembers[TEAM_MEMBERS];
  qhandle_t teamIcon;
  qhandle_t teamIcon_Metal;
  qhandle_t teamIcon_Name;
	int cinematic;
} teamInfo;

typedef struct {
  const char *gameType;
  int gtEnum;
} gameTypeInfo;

typedef struct {
  const char *mapName;
  const char *mapLoadName;
	const char *imageName;
	const char *opponentName;
	int teamMembers;
  int typeBits;
	int cinematic;
	int timeToBeat[MAX_GAMETYPES];
	qhandle_t levelShot;
	qboolean active;
} mapInfo;

typedef struct {
	const char *tierName;
	const char *maps[MAPS_PER_TIER];
	int gameTypes[MAPS_PER_TIER];
	qhandle_t mapHandles[MAPS_PER_TIER];
} tierInfo;

typedef struct serverFilter_s {
	const char *description;
	const char *basedir;
} serverFilter_t;

typedef struct {
	char	adrstr[MAX_ADDRESSLENGTH];
	int		start;
} pinglist_t;


typedef struct serverStatus_s {
	pinglist_t pingList[MAX_PINGREQUESTS];
	int		numqueriedservers;
	int		currentping;
	int		nextpingtime;
	int		maxservers;
	int		refreshtime;
	int		numServers;
	int		sortKey;
	int		sortDir;
	int		lastCount;
	qboolean refreshActive;
	int		currentServer;
	int		displayServers[MAX_DISPLAY_SERVERS];
	int		numDisplayServers;
	int		numPlayersOnServers;
	int		nextDisplayRefresh;
	int		nextSortTime;
	qhandle_t currentServerPreview;
	int		currentServerCinematic;
	int		motdLen;
	int		motdWidth;
	int		motdPaintX;
	int		motdPaintX2;
	int		motdOffset;
	int		motdTime;
	char	motd[MAX_STRING_CHARS];
} serverStatus_t;


typedef struct {
	char		adrstr[MAX_ADDRESSLENGTH];
	char		name[MAX_ADDRESSLENGTH];
	int			startTime;
	int			serverNum;
	qboolean	valid;
} pendingServer_t;

typedef struct {
	int num;
	pendingServer_t server[MAX_SERVERSTATUSREQUESTS];
} pendingServerStatus_t;

typedef struct {
	char address[MAX_ADDRESSLENGTH];
	char *lines[MAX_SERVERSTATUS_LINES][4];
	char text[MAX_SERVERSTATUS_TEXT];
	char pings[MAX_CLIENTS * 3];
	int numLines;
} serverStatusInfo_t;

typedef struct {
	const char *modName;
	const char *modDescr;
} modInfo_t;


typedef struct 
{
	displayContextDef_t uiDC;
	int newHighScoreTime;
	int newBestTime;
	int showPostGameTime;
	qboolean newHighScore;
	qboolean demoAvailable;
	qboolean soundHighScore;
	
	int botIndex;

	int aliasCount;
	aliasInfo aliasList[MAX_ALIASES];

	int redBlue;
	int playerCount;
	int myTeamCount;
	int teamIndex;
	int playerRefresh;
	int playerIndex;
	int playerNumber; 
	int adminActionIndex;
	int adminMessageIndex;
	qboolean teamLeader;
	char playerNames[MAX_CLIENTS][MAX_NAME_LENGTH];
	char teamNames[MAX_CLIENTS][MAX_NAME_LENGTH];
	char adminActionNames[28][MAX_NAME_LENGTH];
	char adminMessageNames[2][MAX_NAME_LENGTH];
	int teamClientNums[MAX_CLIENTS];

	int mapCount;
	mapInfo mapList[MAX_MAPS];


	int tierCount;
	tierInfo tierList[MAX_TIERS];

	int skillIndex;

	modInfo_t modList[MAX_MODS];
	int modCount;
	int modIndex;

	const char *demoList[MAX_DEMOS];
	int demoCount;
	int demoIndex;

	const char *movieList[MAX_MOVIES];
	int movieCount;
	int movieIndex;
	int previewMovie;

	serverStatus_t serverStatus;

	// for the showing the status of a server
	char serverStatusAddress[MAX_ADDRESSLENGTH];
	serverStatusInfo_t serverStatusInfo;
	int nextServerStatusRefresh;

	// to retrieve the status of server to find a player
	pendingServerStatus_t pendingServerStatus;
	char findPlayerName[MAX_STRING_CHARS];
	char foundPlayerServerAddresses[MAX_FOUNDPLAYER_SERVERS][MAX_ADDRESSLENGTH];
	char foundPlayerServerNames[MAX_FOUNDPLAYER_SERVERS][MAX_ADDRESSLENGTH];
	int currentFoundPlayerServer;
	int numFoundPlayerServers;
	int nextFindPlayerRefresh;

	int currentCrosshair;
	int startPostGameTime;

	TIdentity*		identityTeams[TEAM_NUM_TEAMS][MAX_TEAMIDENTITIES];
	int				identitySelected;

	qboolean		inGameLoad;

	char			parentalPassword[MAX_QPATH];

	qboolean		connectPasswordRequest;

	int				outfittingItemGroup;

	int				versionIndex;

	const char*		menusFile;

}	uiInfo_t;


extern uiInfo_t uiInfo;


extern void			UI_Init( void );
extern void			UI_Shutdown( void );
extern void			UI_KeyEvent( int key );
extern void			UI_MouseEvent( int dx, int dy );
extern void			UI_Refresh( int realtime );
extern qboolean		UI_ConsoleCommand( int realTime );
extern void			UI_DrawNamedPic( float x, float y, float width, float height, const char *picname );
extern void			UI_DrawHandlePic( float x, float y, float w, float h, qhandle_t hShader ); 
extern void			UI_FillRect( float x, float y, float width, float height, const float *color );
extern void			UI_DrawRect( float x, float y, float width, float height, const float *color );
extern void     UI_DrawTopBottom(float x, float y, float w, float h);
extern void     UI_DrawSides(float x, float y, float w, float h);
extern void			UI_UpdateScreen( qboolean IsLoading );
extern void			UI_SetColor( const float *rgba );
extern void			UI_LerpColor(vec4_t a, vec4_t b, vec4_t c, float t);
extern void			UI_DrawBannerString( int x, int y, const char* str, int style, vec4_t color );
extern float		UI_ProportionalSizeScale( int style );
extern int			UI_ProportionalStringWidth( const char* str );
extern void			UI_DrawString( int x, int y, const char* str, int style, vec4_t color );
extern void			UI_DrawChar( int x, int y, int ch, int style, vec4_t color );
extern qboolean 	UI_CursorInRect (int x, int y, int width, int height);
extern void			UI_AdjustFrom640( float *x, float *y, float *w, float *h );
extern void			UI_DrawTextBox (int x, int y, int width, int lines);
extern qboolean		UI_IsFullscreen( void );
extern void			UI_SetActiveMenu( uiMenuCommand_t menu );
extern void			UI_PushMenu ( menuframework_s *menu );
extern void			UI_PopMenu (void);
extern void			UI_ForceMenuOff (void);
extern char			*UI_Argv( int arg );
extern char			*UI_Cvar_VariableString( const char *var_name );
extern void			UI_Refresh( int time );
extern void			UI_KeyEvent( int key );
extern qboolean		m_entersound;
extern uiStatic_t	uis;

void UI_DrawText			( float x, float y, qhandle_t font, float scale, vec4_t color, const char* text, int limit, int flags );
void UI_DrawTextWithCursor	( float x, float y, qhandle_t font, float scale, vec4_t color, const char* text, int limit, int flags, int cursorPos, char cursor );

//
// ui_spLevel.c
//
void UI_SPLevelMenu_Cache( void );
void UI_SPLevelMenu( void );
void UI_SPLevelMenu_f( void );
void UI_SPLevelMenu_ReInit( void );

//
// ui_spArena.c
//
void UI_SPArena_Start( const char *arenaInfo );

//
// ui_spPostgame.c
//
void UI_SPPostgameMenu_Cache( void );
void UI_SPPostgameMenu_f( void );

//
// ui_spSkill.c
//
void UI_SPSkillMenu( const char *arenaInfo );
void UI_SPSkillMenu_Cache( void );

//
// ui_syscalls.c
//

void			trap_Parental_Update		( void );
void			trap_Parental_SetPassword	( const char* password );
void			trap_Parental_GetPassword	( char* password, int size );

void			trap_PunkBuster_Enable		( void );
void			trap_PunkBuster_Disable		( void );
int				trap_PunkBuster_IsEnabled	( void );

int				trap_GetTeamCount			( int team );
int				trap_GetTeamScore			( int team );

void			trap_Print( const char *string );
void			trap_Error( const char *string );
int				trap_Milliseconds( void );
void			trap_Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags, float MinValue, float MaxValue );
void			trap_Cvar_Update( vmCvar_t *vmCvar );
void			trap_Cvar_Set( const char *var_name, const char *value );
float			trap_Cvar_VariableValue( const char *var_name );
void			trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void			trap_Cvar_SetValue( const char *var_name, float value );
void			trap_Cvar_Reset( const char *name );
void			trap_Cvar_Create( const char *var_name, const char *var_value, int flags );
void			trap_Cvar_InfoStringBuffer( int bit, char *buffer, int bufsize );
int				trap_Argc( void );
void			trap_Argv( int n, char *buffer, int bufferLength );
void			trap_Cmd_ExecuteText( int exec_when, const char *text );	// don't use EXEC_NOW!
int				trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void			trap_FS_Read( void *buffer, int len, fileHandle_t f );
void			trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void			trap_FS_FCloseFile( fileHandle_t f );
int				trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );
qhandle_t		trap_R_RegisterModel( const char *name );
qhandle_t		trap_R_RegisterSkin( const char *name );
qhandle_t		trap_R_RegisterShaderNoMip( const char *name );
void			trap_R_ClearScene( void );
void			trap_R_AddRefEntityToScene( const refEntity_t *re );
void			trap_R_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts );
void			trap_R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b );
void			trap_R_RenderScene( const refdef_t *fd );
void			trap_R_SetColor( const float *rgba );
void			trap_R_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, const float* color, qhandle_t hShader );
void			trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
void			trap_UpdateScreen( qboolean IsLoading );
int				trap_CM_LerpTag( orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char *tagName );
void			trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum );
sfxHandle_t		trap_S_RegisterSound( const char *sample );
void			trap_Key_KeynumToStringBuf( int keynum, char *buf, int buflen );
void			trap_Key_GetBindingBuf( int keynum, char *buf, int buflen );
void			trap_Key_SetBinding( int keynum, const char *binding );
qboolean		trap_Key_IsDown( int keynum );
qboolean		trap_Key_GetOverstrikeMode( void );
void			trap_Key_SetOverstrikeMode( qboolean state );
void			trap_Key_ClearStates( void );
int				trap_Key_GetCatcher( void );
void			trap_Key_SetCatcher( int catcher );
void			trap_GetClipboardData( char *buf, int bufsize );
void			trap_GetClientState( uiClientState_t *state );
void			trap_GetGlconfig( glconfig_t *glconfig );
int				trap_GetConfigString( int index, char* buff, int buffsize );
int				trap_LAN_GetServerCount( int source );
void			trap_LAN_GetServerAddressString( int source, int n, char *buf, int buflen );
void			trap_LAN_GetServerInfo( int source, int n, char *buf, int buflen );
int				trap_LAN_GetServerPing( int source, int n );
int				trap_LAN_GetPingQueueCount( void );
void			trap_LAN_ClearPing( int n );
void			trap_LAN_GetPing( int n, char *buf, int buflen, int *pingtime );
void			trap_LAN_GetPingInfo( int n, char *buf, int buflen );
void			trap_LAN_LoadCachedServers();
void			trap_LAN_SaveCachedServers();
void			trap_LAN_MarkServerVisible(int source, int n, qboolean visible);
int				trap_LAN_ServerIsVisible( int source, int n);
qboolean		trap_LAN_UpdateVisiblePings( int source );
int				trap_LAN_AddServer(int source, const char *name, const char *addr);
void			trap_LAN_RemoveServer(int source, const char *addr);
void			trap_LAN_ResetPings(int n);
int				trap_LAN_ServerStatus( const char *serverAddress, char *serverStatus, int maxLen );
int				trap_LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 );
int				trap_MemoryRemaining( void );
void			trap_GetCDKey( char *buf, int buflen );
void			trap_SetCDKey( char *buf );
qhandle_t		trap_R_RegisterFont(const char *pFontname);
int				trap_R_GetTextWidth			( const char* text, qhandle_t font, float scale, int limit );
int				trap_R_GetTextHeight		( const char* text, qhandle_t font, float scale, int limit );
void			trap_R_DrawText				( int x, int y, qhandle_t font, float scale, vec4_t color, const char* text, int limit, int flags );
void			trap_R_DrawTextWithCursor	( int x, int y, qhandle_t font, float scale, vec4_t color, const char* text, int limit, int flags, int cursorPos, char cursor );
void			trap_S_StopBackgroundTrack	( void );
void			trap_S_StartBackgroundTrack	( const char *intro, const char *loop, qboolean bReturnWithoutStarting);
int				trap_CIN_PlayCinematic		( const char *arg0, int xpos, int ypos, int width, int height, int bits);
e_status		trap_CIN_StopCinematic		( int handle );
e_status		trap_CIN_RunCinematic		( int handle );
void			trap_CIN_DrawCinematic		( int handle );
void			trap_CIN_SetExtents			( int handle, int x, int y, int w, int h );
int				trap_RealTime				( qtime_t *qtime );
void			trap_R_RemapShader			( const char *oldShader, const char *newShader, const char *timeOffset );
qboolean		trap_VerifyCDKey			( const char *key );

// CG specific API access
void		trap_G2_ListModelSurfaces(void *ghlInfo);
void		trap_G2_ListModelBones(void *ghlInfo, int frame);
int			trap_G2API_AddBolt(void *ghoul2, const int modelIndex, const char *boneName);
qboolean	trap_G2API_RemoveBolt(void *ghlInfo, const int modelIndex, const int index);
qboolean	trap_G2API_AttachG2Model(void *ghoul2From, int modelFrom, void *ghoul2To, int toBoltIndex, int toModel);
void		trap_G2_SetGhoul2ModelIndexes(void *ghoul2, qhandle_t *modelList, qhandle_t *skinList);
qboolean	trap_G2_HaveWeGhoul2Models(void *ghoul2);
qboolean	trap_G2API_GetBoltMatrix(void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix,
								const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale);
int			trap_G2API_InitGhoul2Model(void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin,
						  qhandle_t customShader, int modelFlags, int lodBias);

int			trap_G2API_CopyGhoul2Instance(void *g2From, void *g2To, int modelIndex);
int			trap_G2API_CopySpecificGhoul2Model(void *g2From, int modelFrom, void *g2To, int modelTo);
void		trap_G2API_DuplicateGhoul2Instance(void *g2From, void **g2To);
qboolean	trap_G2API_RemoveGhoul2Model(void **ghlInfo, int modelIndex);
void		trap_G2API_CleanGhoul2Models(void **ghoul2Ptr);

qboolean	trap_G2API_SetBoneAngles(void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags,
								const int up, const int right, const int forward, qhandle_t *modelList,
								int blendTime , int currentTime );
char		*trap_G2API_GetGLAName(void *ghoul2, int modelIndex);
qboolean	trap_G2API_SetBoneAnim(void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame,
							  const int flags, const float animSpeed, const int currentTime, const float setFrame , const int blendTime );
qboolean	trap_G2API_SetSurfaceOnOff(void *ghoul2, const int modelIndex, const char *surfaceName, const int flags);
qboolean	trap_G2API_GetAnimFileNameIndex (TGhoul2 ghoul2, qhandle_t modelIndex, const char* name );
qhandle_t	trap_G2API_RegisterSkin ( const char *skinName, int numPairs, const char *skinPairs);
qboolean	trap_G2API_SetSkin		( TGhoul2 ghoul2, int modelIndex, qhandle_t customSkin);

void		*trap_VM_LocalAlloc ( int size );
void		*trap_VM_LocalAllocUnaligned ( int size );			// WARNING!!!! USE WITH CAUTION!!! BEWARE OF DOG!!!
void		*trap_VM_LocalTempAlloc( int size );
void		trap_VM_LocalTempFree( int size );					// free must be in opposite order of allocation!
const char	*trap_VM_LocalStringAlloc ( const char *source );

qboolean	trap_NET_Available(void);

void		trap_Version_GetDescription(char *description, int length);
int			trap_Version_GetNumSites(void);
void		trap_Version_GetSite(int index, char *site, int length);
void		trap_Version_Download(int index);

//
// ui_addbots.c
//
void UI_AddBots_Cache( void );
void UI_AddBotsMenu( void );

//
// ui_removebots.c
//
void UI_RemoveBots_Cache( void );
void UI_RemoveBotsMenu( void );

//
// ui_teamorders.c
//
extern void UI_TeamOrdersMenu( void );
extern void UI_TeamOrdersMenu_f( void );
extern void UI_TeamOrdersMenu_Cache( void );

//
// ui_loadconfig.c
//
void UI_LoadConfig_Cache( void );
void UI_LoadConfigMenu( void );

//
// ui_saveconfig.c
//
void UI_SaveConfigMenu_Cache( void );
void UI_SaveConfigMenu( void );

//
// ui_display.c
//
void UI_DisplayOptionsMenu_Cache( void );
void UI_DisplayOptionsMenu( void );

//
// ui_sound.c
//
void UI_SoundOptionsMenu_Cache( void );
void UI_SoundOptionsMenu( void );

//
// ui_network.c
//
void UI_NetworkOptionsMenu_Cache( void );
void UI_NetworkOptionsMenu( void );

//
// ui_gameinfo.c
//
typedef enum {
	AWARD_ACCURACY,
	AWARD_IMPRESSIVE,
	AWARD_EXCELLENT,
	AWARD_GAUNTLET,
	AWARD_FRAGS,
	AWARD_PERFECT
} awardType_t;

const char *UI_GetArenaInfoByNumber( int num );
const char *UI_GetArenaInfoByMap( const char *map );
const char *UI_GetSpecialArenaInfo( const char *tag );
int UI_GetNumArenas( void );
int UI_GetNumSPArenas( void );
int UI_GetNumSPTiers( void );

char *UI_GetBotInfoByNumber( int num );
char *UI_GetBotInfoByName( const char *name );
int UI_GetNumBots( void );
void UI_LoadBots( void );
char *UI_GetBotNameByNumber( int num );

void UI_GetBestScore( int level, int *score, int *skill );
void UI_SetBestScore( int level, int score );
int UI_TierCompleted( int levelWon );
qboolean UI_ShowTierVideo( int tier );
qboolean UI_CanShowTierVideo( int tier );
int  UI_GetCurrentGame( void );
void UI_NewGame( void );
void UI_LogAwardData( int award, int data );
int UI_GetAwardLevel( int award );

void UI_SPUnlock_f( void );
void UI_SPUnlockMedals_f( void );

void UI_InitGameinfo( void );

//
// ui_login.c
//
void Login_Cache( void );
void UI_LoginMenu( void );

//
// ui_signup.c
//
void Signup_Cache( void );
void UI_SignupMenu( void );

//
// ui_rankstatus.c
//
void RankStatus_Cache( void );
void UI_RankStatusMenu( void );


// new ui 

#define ASSET_BACKGROUND "uiBackground"

// for tracking sp game info in Team Arena
typedef struct postGameInfo_s {
	int score;
	int redScore;
	int blueScore;
	int perfects;
	int accuracy;
	int impressives;
	int excellents;
	int defends;
	int assists;
	int gauntlets;
	int	captures;
	int time;
	int timeBonus;
	int shutoutBonus;
	int skillBonus;
	int baseScore;
} postGameInfo_t;



#endif
