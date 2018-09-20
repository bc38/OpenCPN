/***************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2018 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/

#include <wx/tokenzr.h>

#include "ConfigMgr.h"

#include <wx/filename.h>
#include <wx/fileconf.h>


#include "dychart.h"

#include <stdlib.h>
//#include <math.h>
#include <time.h>
#include <locale>
#include <list>

#include <wx/listimpl.cpp>
#include <wx/progdlg.h>

#include "chart1.h"
#include "navutil.h"
#include "chcanv.h"
#include "georef.h"
#include "cutil.h"
#include "styles.h"
#include "routeman.h"
#include "routeprop.h"
#include "s52utils.h"
#include "chartbase.h"
#include "ocpndc.h"
#include "geodesic.h"
#include "datastream.h"
#include "multiplexer.h"
#include "ais.h"
#include "Route.h"
#include "Select.h"
#include "FontMgr.h"
#include "OCPN_Sound.h"
#include "Layer.h"
#include "NavObjectCollection.h"
#include "NMEALogWindow.h"
#include "AIS_Decoder.h"
#include "OCPNPlatform.h"
#include "Track.h"
#include "chartdb.h"
#include "CanvasConfig.h"

#ifdef USE_S57
#include "s52plib.h"
#include "cm93.h"
#endif

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

// Global statics
//    Statics

extern OCPNPlatform     *g_Platform;
extern ChartCanvas      *cc1;
extern MyFrame          *gFrame;

extern double           g_ChartNotRenderScaleFactor;
extern int              g_restore_stackindex;
extern int              g_restore_dbindex;
extern RouteList        *pRouteList;
extern TrackList        *pTrackList;
extern LayerList        *pLayerList;
extern int              g_LayerIdx;
extern Select           *pSelect;
extern MyConfig         *pConfig;
extern ArrayOfCDI       g_ChartDirArray;
extern double           vLat, vLon, gLat, gLon;
extern double           kLat, kLon;
extern double           initial_scale_ppm, initial_rotation;
extern ColorScheme      global_color_scheme;
extern int              g_nbrightness;
extern bool             g_bShowTrue, g_bShowMag;
extern double           g_UserVar;
extern bool             g_bShowStatusBar;
extern bool             g_bUIexpert;
extern bool             g_bFullscreen;
extern int              g_nDepthUnitDisplay;

extern wxToolBarBase    *toolBar;

extern wxArrayOfConnPrm *g_pConnectionParams;

extern wxString         g_csv_locn;
extern wxString         g_SENCPrefix;
extern wxString         g_UserPresLibData;

extern AIS_Decoder      *g_pAIS;
extern wxString         *pInit_Chart_Dir;
extern wxString         gWorldMapLocation;
extern WayPointman      *pWayPointMan;
extern Routeman         *g_pRouteMan;
extern RouteProp        *pRoutePropDialog;

extern bool             s_bSetSystemTime;
extern bool             g_bDisplayGrid;         //Flag indicating if grid is to be displayed
extern bool             g_bPlayShipsBells;
extern int              g_iSoundDeviceIndex;
extern bool             g_bFullscreenToolbar;
extern bool             g_bShowLayers;
extern bool             g_bTransparentToolbar;
extern bool             g_bPermanentMOBIcon;

extern bool             g_bShowDepthUnits;
extern bool             g_bAutoAnchorMark;
extern bool             g_bskew_comp;
extern bool             g_bopengl;
extern bool             g_bdisable_opengl;
extern bool             g_bSoftwareGL;
extern bool             g_bShowFPS;
extern bool             g_bsmoothpanzoom;
extern bool             g_fog_overzoom;
extern double           g_overzoom_emphasis_base;
extern bool             g_oz_vector_scale;

extern bool             g_bShowOutlines;
extern bool             g_bShowActiveRouteHighway;
extern bool             g_bShowRouteTotal;
extern int              g_nNMEADebug;
extern int              g_nAWDefault;
extern int              g_nAWMax;
extern int              g_nTrackPrecision;

extern int              g_iSDMMFormat;
extern int              g_iDistanceFormat;
extern int              g_iSpeedFormat;

extern int              g_nframewin_x;
extern int              g_nframewin_y;
extern int              g_nframewin_posx;
extern int              g_nframewin_posy;
extern bool             g_bframemax;

extern double           g_PlanSpeed;
extern wxString         g_VisibleLayers;
extern wxString         g_InvisibleLayers;
extern wxRect           g_blink_rect;

extern wxArrayString    *pMessageOnceArray;

// LIVE ETA OPTION
extern bool             g_bShowLiveETA;
extern double           g_defaultBoatSpeed;
extern double           g_defaultBoatSpeedUserUnit;

//    AIS Global configuration
extern bool             g_bCPAMax;
extern double           g_CPAMax_NM;
extern bool             g_bCPAWarn;
extern double           g_CPAWarn_NM;
extern bool             g_bTCPA_Max;
extern double           g_TCPA_Max;
extern bool             g_bMarkLost;
extern double           g_MarkLost_Mins;
extern bool             g_bRemoveLost;
extern double           g_RemoveLost_Mins;
extern bool             g_bShowCOG;
extern double           g_ShowCOG_Mins;
extern bool             g_bAISShowTracks;
extern bool             g_bTrackCarryOver;
extern bool             g_bTrackDaily;
extern int              g_track_rotate_time;
extern int              g_track_rotate_time_type;
extern double           g_AISShowTracks_Mins;
extern bool             g_bHideMoored;
extern double           g_ShowMoored_Kts;
extern bool             g_bAllowShowScaled;
extern bool             g_bShowScaled;
extern int              g_ShowScaled_Num;
extern bool             g_bAIS_CPA_Alert;
extern bool             g_bAIS_CPA_Alert_Audio;
extern int              g_ais_alert_dialog_x, g_ais_alert_dialog_y;
extern int              g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;
extern int              g_ais_query_dialog_x, g_ais_query_dialog_y;
extern wxString         g_sAIS_Alert_Sound_File;
extern bool             g_bAIS_CPA_Alert_Suppress_Moored;
extern bool             g_bAIS_ACK_Timeout;
extern double           g_AckTimeout_Mins;
extern wxString         g_AisTargetList_perspective;
extern int              g_AisTargetList_range;
extern int              g_AisTargetList_sortColumn;
extern bool             g_bAisTargetList_sortReverse;
extern wxString         g_AisTargetList_column_spec;
extern bool             g_bShowAreaNotices;
extern bool             g_bDrawAISSize;
extern bool             g_bShowAISName;
extern int              g_Show_Target_Name_Scale;
extern bool             g_bWplIsAprsPosition;
extern bool             g_benableAISNameCache;
extern int              g_ScaledNumWeightSOG;
extern int              g_ScaledNumWeightCPA;
extern int              g_ScaledNumWeightTCPA;
extern int              g_ScaledNumWeightRange;
extern int              g_ScaledNumWeightSizeOfT;
extern int              g_ScaledSizeMinimal;

extern int              g_S57_dialog_sx, g_S57_dialog_sy;

extern int              g_iNavAidRadarRingsNumberVisible;
extern float            g_fNavAidRadarRingsStep;
extern int              g_pNavAidRadarRingsStepUnits;
extern int              g_iWaypointRangeRingsNumber;
extern float            g_fWaypointRangeRingsStep;
extern int              g_iWaypointRangeRingsStepUnits;
extern wxColour         g_colourWaypointRangeRingsColour;
extern bool             g_bWayPointPreventDragging;
extern bool             g_bConfirmObjectDelete;
extern wxColour         g_colourOwnshipRangeRingsColour;

extern bool             g_bEnableZoomToCursor;
extern wxString         g_toolbarConfig;
extern wxString         g_toolbarConfigSecondary;
extern double           g_TrackIntervalSeconds;
extern double           g_TrackDeltaDistance;
extern int              gps_watchdog_timeout_ticks;

extern int              g_nCacheLimit;
extern int              g_memCacheLimit;

extern bool             g_bGDAL_Debug;
extern bool             g_bDebugCM93;
extern bool             g_bDebugS57;

extern double           g_ownship_predictor_minutes;
extern double           g_ownship_HDTpredictor_miles;

extern bool             g_own_ship_sog_cog_calc;
extern int              g_own_ship_sog_cog_calc_damp_sec;

#ifdef USE_S57
extern s52plib          *ps52plib;
#endif

extern int              g_cm93_zoom_factor;
extern bool             g_b_legacy_input_filter_behaviour;
extern bool             g_bShowDetailSlider;
extern int              g_detailslider_dialog_x, g_detailslider_dialog_y;

extern bool             g_bUseGreenShip;

extern bool             g_b_overzoom_x;                      // Allow high overzoom
extern int              g_nautosave_interval_seconds;
extern int              g_OwnShipIconType;
extern double           g_n_ownship_length_meters;
extern double           g_n_ownship_beam_meters;
extern double           g_n_gps_antenna_offset_y;
extern double           g_n_gps_antenna_offset_x;
extern int              g_n_ownship_min_mm;
extern double           g_n_arrival_circle_radius;

extern bool             g_bPreserveScaleOnX;
extern bool             g_bsimplifiedScalebar;

extern bool             g_bUseRMC;
extern bool             g_bUseGLL;

extern wxString         g_locale;
extern wxString         g_localeOverride;

extern bool             g_bUseRaster;
extern bool             g_bUseVector;
extern bool             g_bUseCM93;

extern bool             g_bCourseUp;
extern bool             g_bLookAhead;
extern int              g_COGAvgSec;
extern bool             g_bMagneticAPB;
extern bool             g_bShowChartBar;

extern int              g_MemFootSec;
extern int              g_MemFootMB;

extern int              g_nCOMPortCheck;

extern bool             g_bbigred;

extern wxString         g_AW1GUID;
extern wxString         g_AW2GUID;
extern int              g_BSBImgDebug;

extern int             n_NavMessageShown;
extern wxString        g_config_version_string;

extern bool             g_bAISRolloverShowClass;
extern bool             g_bAISRolloverShowCOG;
extern bool             g_bAISRolloverShowCPA;

extern bool             g_bDebugGPSD;

extern bool             g_bfilter_cogsog;
extern int              g_COGFilterSec;
extern int              g_SOGFilterSec;

extern int              g_navobjbackups;

extern bool             g_bQuiltEnable;
extern bool             g_bFullScreenQuilt;
extern bool             g_bQuiltStart;

extern int              g_SkewCompUpdatePeriod;

extern int              g_maintoolbar_x;
extern int              g_maintoolbar_y;
extern long             g_maintoolbar_orient;

extern int              g_GPU_MemSize;

extern int              g_lastClientRectx;
extern int              g_lastClientRecty;
extern int              g_lastClientRectw;
extern int              g_lastClientRecth;

extern bool             g_bHighliteTracks;
extern int              g_cog_predictor_width;
extern int              g_ais_cog_predictor_width;

extern int              g_route_line_width;
extern int              g_track_line_width;
extern wxColour         g_colourTrackLineColour;
extern wxString         g_default_wp_icon;

extern ChartGroupArray  *g_pGroupArray;
extern int              g_GroupIndex;

extern bool             g_bDebugOGL;
extern int              g_current_arrow_scale;
extern int              g_tide_rectangle_scale;
extern int              g_tcwin_scale;
extern wxString         g_GPS_Ident;
extern bool             g_bGarminHostUpload;
extern wxString         g_uploadConnection;

extern ocpnStyle::StyleManager* g_StyleManager;
extern wxArrayString    TideCurrentDataSet;
extern wxString         g_TCData_Dir;
extern Multiplexer      *g_pMUX;
extern bool             portaudio_initialized;

extern bool             g_btouch;
extern bool             g_bresponsive;

extern bool             bGPSValid;              // for track recording
extern bool             g_bGLexpert;

extern int              g_SENC_LOD_pixels;
extern ArrayOfMMSIProperties   g_MMSI_Props_Array;

extern int              g_chart_zoom_modifier;
extern int              g_chart_zoom_modifier_vector;

extern int              g_NMEAAPBPrecision;

extern wxString         g_TalkerIdText;
extern int              g_maxWPNameLength;

extern bool             g_bAdvanceRouteWaypointOnArrivalOnly;
extern double           g_display_size_mm;
extern double           g_config_display_size_mm;
extern bool             g_config_display_size_manual;

extern bool             g_benable_rotate;
extern bool             g_bEmailCrashReport;

extern int              g_default_font_size;

extern bool             g_bAutoHideToolbar;
extern int              g_nAutoHideToolbar;
extern int              g_GUIScaleFactor;
extern int              g_ChartScaleFactor;
extern float            g_ChartScaleFactorExp;
extern int              g_ShipScaleFactor;
extern float            g_ShipScaleFactorExp;

extern bool             g_bInlandEcdis;
extern int              g_iENCToolbarPosX;
extern int              g_iENCToolbarPosY;

extern bool             g_bSpaceDropMark;

extern bool             g_bShowTide;
extern bool             g_bShowCurrent;

extern bool             g_benableUDPNullHeader;
extern bool             g_bShowMenuBar;
extern bool             g_bShowCompassWin;

extern wxString         g_uiStyle;
extern bool             g_useMUI;

extern int              g_nCPUCount;

extern bool             g_bDarkDecorations;
extern unsigned int     g_canvasConfig;
extern arrayofCanvasConfigPtr g_canvasConfigArray;

#ifdef ocpnUSE_GL
extern ocpnGLOptions g_GLOptions;
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif



ConfigMgr * ConfigMgr::instance = NULL;

class OCPNConfigObject
{
public:
    OCPNConfigObject();
    ~OCPNConfigObject();

    OCPNConfigObject( int canvas_config );
    
    void Init();
    wxPanel *GetSettingsPanel();
    
    int m_canvasConfig;
    wxString m_GUID;
    wxString templateFileName;
    wxString m_title;
    wxString m_description;
    
    
};

OCPNConfigObject::OCPNConfigObject()
{
    Init();
}

OCPNConfigObject::OCPNConfigObject( int canvas_config )
{
    Init();
    m_canvasConfig = canvas_config;
}

OCPNConfigObject::~OCPNConfigObject()
{
}

void OCPNConfigObject::Init()
{
    m_canvasConfig = 0;
}

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ConfigObjectList);

//--------------------------------------------------------------------
//   Private ( XML encoded ) catalog of available configurations
//--------------------------------------------------------------------

class OCPNConfigCatalog : public pugi::xml_document
{
public:
    OCPNConfigCatalog();
    ~OCPNConfigCatalog();
    
    bool AddConfig( OCPNConfigObject *config, unsigned int flags );
    
    
    void SetRootConfigNode(void);
    bool IsOpenCPN();
    bool SaveFile( const wxString filename );
    bool LoadFile( const wxString filename );
    
    pugi::xml_node      m_config_root;
};

OCPNConfigCatalog::OCPNConfigCatalog()
: pugi::xml_document()
{
}

OCPNConfigCatalog::~OCPNConfigCatalog()
{
}

void OCPNConfigCatalog::SetRootConfigNode(void)
{
    if(!strlen(m_config_root.name())) {
        m_config_root = append_child("configs");
        m_config_root.append_attribute ( "version" ) = "1.0";
        m_config_root.append_attribute ( "creator" ) = "OpenCPN";
        m_config_root.append_attribute( "xmlns:xsi" ) = "http://www.w3.org/2001/XMLSchema-instance";
        m_config_root.append_attribute( "xmlns" ) = "http://www.topografix.com/GPX/1/1";
        m_config_root.append_attribute( "xmlns:gpxx" ) =  "http://www.garmin.com/xmlschemas/GpxExtensions/v3";
        m_config_root.append_attribute( "xsi:schemaLocation" ) = "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd";
        m_config_root.append_attribute( "xmlns:opencpn" ) = "http://www.opencpn.org";
    }
}

bool OCPNConfigCatalog::IsOpenCPN()
{
    for (pugi::xml_attribute attr = root().first_child().first_attribute(); attr; attr = attr.next_attribute())
        if( !strcmp(attr.name(), "creator") && !strcmp(attr.value(), "OpenCPN") )
            return true;
        return false;
}

bool OCPNConfigCatalog::SaveFile( const wxString filename )
{
    save_file(filename.fn_str(), "  ");
    return true;
}

bool OCPNConfigCatalog::LoadFile( const wxString filename )
{
    load_file( filename.fn_str() );
    m_config_root =  this->child("configs");
    return true;
}

bool OCPNConfigCatalog::AddConfig( OCPNConfigObject *config, unsigned int flags )
{
    pugi::xml_node node = m_config_root.append_child("config");
    
    node.append_attribute("GUID") = config->m_GUID.mb_str();
    node.append_attribute("title") = config->m_title.mb_str();
    node.append_attribute("description") = config->m_description.mb_str();
    node.append_attribute("templateFile") = config->templateFileName.mb_str();

    
    return true;
}
    
    


//--------------------------------------------------------------------
//   ConfigPanel implementation
//--------------------------------------------------------------------
    
ConfigPanel::ConfigPanel(OCPNConfigObject *config, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
    :wxPanel(parent, id, pos, size, wxBORDER_NONE)

{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
    
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Title")));
    mainSizer->Add(new wxStaticText(this, wxID_ANY, config->m_title));
    
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("GUID")));
    mainSizer->Add(new wxStaticText(this, wxID_ANY, config->m_GUID));
    
    SetMinSize(wxSize(-1, 8 * GetCharHeight()) );
}

ConfigPanel::~ConfigPanel()
{
}
    

//--------------------------------------------------------------------
//   ConfigMgr implementation
//   Singleton Pattern
//--------------------------------------------------------------------
    

ConfigMgr & ConfigMgr::Get()
{
    if (!instance)
        instance = new ConfigMgr;
    return *instance;
}

void ConfigMgr::Shutdown()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
}

ConfigMgr::ConfigMgr()
{
    Init();
    
    // Load any existing configs from the catalog
    LoadCatalog();
    
    

}

ConfigMgr::~ConfigMgr()
{
    delete configList;
}

void ConfigMgr::Init()
{
    m_configDir = g_Platform->GetPrivateDataDir() + wxFileName::GetPathSeparator() + _T("Configs") + wxFileName::GetPathSeparator();
    if(!wxFileName::DirExists( m_configDir )){
        wxFileName::Mkdir( m_configDir );
    }
    
    m_configCatalogName = g_Platform->GetPrivateDataDir() + wxFileName::GetPathSeparator() + _T("Configs") + wxFileName::GetPathSeparator() +_T("configs.xml");
    
    //Create the catalog, if necessary
    if(!wxFileExists(m_configCatalogName)){
        
        wxLogMessage( _T("Creating new Configs catalog: ") + m_configCatalogName );
        
        OCPNConfigCatalog *cat = new OCPNConfigCatalog();
        cat->SetRootConfigNode();
        cat->SaveFile( m_configCatalogName);
        delete cat;
    }
    
    m_configCatalog = new OCPNConfigCatalog();
    
    configList = new ConfigObjectList;
    
}

bool ConfigMgr::LoadCatalog()
{
    wxLogMessage( _T("Loading Configs catalog: ") + m_configCatalogName );
    m_configCatalog->LoadFile( m_configCatalogName );
    
    return true;
    
}

bool ConfigMgr::SaveCatalog()
{
    m_configCatalog->SaveFile( m_configCatalogName);
    
    return true;
}

wxString ConfigMgr::CreateNamedConfig( wxString title, wxString description )
{
    wxString GUID;
    
    // Must have title
    if( title.IsEmpty())
        return GUID;
    
    OCPNConfigObject *pConfig = new OCPNConfigObject;
    
    //Create a new GUID for this config
    GUID = GetUUID();
    pConfig->m_GUID = GUID;
    pConfig->m_title = title;
    pConfig->m_description = description;
    
    // create template file name
    pConfig->templateFileName = _T("OCPNTemplate-") + GUID + _T(".conf");
    
    //  Save the template contents
    wxString templateFullFileName = GetConfigDir() + pConfig->templateFileName;
    if(!SaveTemplate( templateFullFileName )){
        wxLogMessage(_T("Unable to save template titled: ") + title + _T(" as file: ") + templateFullFileName);
        delete pConfig;
        return _T("");
    }
    
    // Add this config to the catalog
    if(!m_configCatalog->AddConfig( pConfig, 0 )){
        wxLogMessage(_T("Unable to add config to catalog...Title: ") + title);
        delete pConfig;
        return _T("");
    }
    
    // Add to the class list of configs
    configList->Append(pConfig);
    
    SaveCatalog();
    
    return GUID;
}

wxPanel *ConfigMgr::GetConfigPanel( wxWindow *parent, wxString GUID )
{
    wxPanel *retPanel = NULL;
    
    // Find the GUID-matching config in the member list
    OCPNConfigObject *config = NULL;
    for ( ConfigObjectList::Node *node = configList->GetFirst(); node; node = node->GetNext() )
    {
        OCPNConfigObject *look = node->GetData();
        if(look->m_GUID == GUID){
            config = look;
            break;
        }
    }
    
    //  Found it?
    if(config){
        retPanel = new ConfigPanel( config, parent );
    }
        

    return retPanel;
    
}



int GetRandomNumber(int range_min, int range_max)
{
    long u = (long)wxRound(((double)rand() / ((double)(RAND_MAX) + 1) * (range_max - range_min)) + range_min);
    return (int)u;
}

// RFC4122 version 4 compliant random UUIDs generator.
wxString ConfigMgr::GetUUID(void)
{
    wxString str;
    struct {
        int time_low;
        int time_mid;
        int time_hi_and_version;
        int clock_seq_hi_and_rsv;
        int clock_seq_low;
        int node_hi;
        int node_low;
    } uuid;
    
    uuid.time_low = GetRandomNumber(0, 2147483647);//FIXME: the max should be set to something like MAXINT32, but it doesn't compile un gcc...
    uuid.time_mid = GetRandomNumber(0, 65535);
    uuid.time_hi_and_version = GetRandomNumber(0, 65535);
    uuid.clock_seq_hi_and_rsv = GetRandomNumber(0, 255);
    uuid.clock_seq_low = GetRandomNumber(0, 255);
    uuid.node_hi = GetRandomNumber(0, 65535);
    uuid.node_low = GetRandomNumber(0, 2147483647);
    
    /* Set the two most significant bits (bits 6 and 7) of the
     * clock_seq_hi_and_rsv to zero and one, respectively. */
    uuid.clock_seq_hi_and_rsv = (uuid.clock_seq_hi_and_rsv & 0x3F) | 0x80;
    
    /* Set the four most significant bits (bits 12 through 15) of the
     * time_hi_and_version field to 4 */
    uuid.time_hi_and_version = (uuid.time_hi_and_version & 0x0fff) | 0x4000;
    
    str.Printf(_T("%08x-%04x-%04x-%02x%02x-%04x%08x"),
                  uuid.time_low,
                  uuid.time_mid,
                  uuid.time_hi_and_version,
                  uuid.clock_seq_hi_and_rsv,
                  uuid.clock_seq_low,
                  uuid.node_hi,
                  uuid.node_low);
    
    return str;
}


bool ConfigMgr::SaveTemplate( wxString fileName)
{
    //  Assuming the file exists, and is empty....
    
    //  Create a private wxFileConfig object
    wxFileConfig *conf = new wxFileConfig( _T(""), _T(""), fileName, _T (""),  wxCONFIG_USE_LOCAL_FILE );
    
    //  Write out all the elements of a config template....
    
    //  Temporarily suppress logging of trivial non-fatal wxLogSysError() messages provoked by Android security...
    #ifdef __OCPN__ANDROID__    
    wxLogNull logNo;
    #endif    
    
    
    //    Global options and settings
    conf->SetPath( _T ( "/Settings" ) );
    
    conf->Write( _T ( "InlandEcdis" ), g_bInlandEcdis );
    conf->Write( _T ( "DarkDecorations"), g_bDarkDecorations );
    conf->Write( _T ( "UIexpert" ), g_bUIexpert );
    conf->Write( _T( "SpaceDropMark" ), g_bSpaceDropMark );
    
    conf->Write( _T ( "ShowStatusBar" ), g_bShowStatusBar );
    #ifndef __WXOSX__
    conf->Write( _T ( "ShowMenuBar" ), g_bShowMenuBar );
    #endif
    conf->Write( _T ( "DefaultFontSize" ), g_default_font_size );
    
    conf->Write( _T ( "Fullscreen" ), g_bFullscreen );
    conf->Write( _T ( "ShowCompassWindow" ), g_bShowCompassWin );
    conf->Write( _T ( "SetSystemTime" ), s_bSetSystemTime );
    conf->Write( _T ( "ShowGrid" ), g_bDisplayGrid );
    conf->Write( _T ( "PlayShipsBells" ), g_bPlayShipsBells );
    conf->Write( _T ( "SoundDeviceIndex" ), g_iSoundDeviceIndex );
    conf->Write( _T ( "FullscreenToolbar" ), g_bFullscreenToolbar );
    conf->Write( _T ( "TransparentToolbar" ), g_bTransparentToolbar );
    conf->Write( _T ( "PermanentMOBIcon" ), g_bPermanentMOBIcon );
    conf->Write( _T ( "ShowLayers" ), g_bShowLayers );
    conf->Write( _T ( "AutoAnchorDrop" ), g_bAutoAnchorMark );
    conf->Write( _T ( "ShowChartOutlines" ), g_bShowOutlines );
    conf->Write( _T ( "ShowActiveRouteTotal" ), g_bShowRouteTotal );
    conf->Write( _T ( "ShowActiveRouteHighway" ), g_bShowActiveRouteHighway );
    conf->Write( _T ( "SDMMFormat" ), g_iSDMMFormat );
    conf->Write( _T ( "ShowChartBar" ), g_bShowChartBar );
    
    conf->Write( _T ( "GUIScaleFactor" ), g_GUIScaleFactor );
    conf->Write( _T ( "ChartObjectScaleFactor" ), g_ChartScaleFactor );
    conf->Write( _T ( "ShipScaleFactor" ), g_ShipScaleFactor );
    
    conf->Write( _T ( "ShowTrue" ), g_bShowTrue );
    conf->Write( _T ( "ShowMag" ), g_bShowMag );
    conf->Write( _T ( "UserMagVariation" ), wxString::Format( _T("%.2f"), g_UserVar ) );
    
    conf->Write( _T ( "CM93DetailFactor" ), g_cm93_zoom_factor );
    conf->Write( _T ( "CM93DetailZoomPosX" ), g_detailslider_dialog_x );
    conf->Write( _T ( "CM93DetailZoomPosY" ), g_detailslider_dialog_y );
    conf->Write( _T ( "ShowCM93DetailSlider" ), g_bShowDetailSlider );
    
    conf->Write( _T ( "SkewToNorthUp" ), g_bskew_comp );
    conf->Write( _T ( "OpenGL" ), g_bopengl );
    conf->Write( _T ( "SoftwareGL" ), g_bSoftwareGL );
    conf->Write( _T ( "ShowFPS" ), g_bShowFPS );
    
    conf->Write( _T ( "ZoomDetailFactor" ), g_chart_zoom_modifier );
    conf->Write( _T ( "ZoomDetailFactorVector" ), g_chart_zoom_modifier_vector );
    
    
    #ifdef ocpnUSE_GL
    /* opengl options */
    conf->Write( _T ( "UseAcceleratedPanning" ), g_GLOptions.m_bUseAcceleratedPanning );
    
    conf->Write( _T ( "GPUTextureCompression" ), g_GLOptions.m_bTextureCompression);
    conf->Write( _T ( "GPUTextureCompressionCaching" ), g_GLOptions.m_bTextureCompressionCaching);
    conf->Write( _T ( "GPUTextureDimension" ), g_GLOptions.m_iTextureDimension );
    conf->Write( _T ( "GPUTextureMemSize" ), g_GLOptions.m_iTextureMemorySize );
    conf->Write( _T ( "PolygonSmoothing" ), g_GLOptions.m_GLPolygonSmoothing);
    conf->Write( _T ( "LineSmoothing" ), g_GLOptions.m_GLLineSmoothing);
    #endif
    conf->Write( _T ( "SmoothPanZoom" ), g_bsmoothpanzoom );
    
    conf->Write( _T ( "CourseUpMode" ), g_bCourseUp );
    if (!g_bInlandEcdis )
        conf->Write( _T ( "LookAheadMode" ), g_bLookAhead );
    conf->Write( _T ( "COGUPAvgSeconds" ), g_COGAvgSec );
    conf->Write( _T ( "UseMagAPB" ), g_bMagneticAPB );
    
    conf->Write( _T ( "OwnshipCOGPredictorMinutes" ), g_ownship_predictor_minutes );
    conf->Write( _T ( "OwnshipCOGPredictorWidth" ), g_cog_predictor_width );
    conf->Write( _T ( "OwnshipHDTPredictorMiles" ), g_ownship_HDTpredictor_miles );
    conf->Write( _T ( "OwnShipIconType" ), g_OwnShipIconType );
    conf->Write( _T ( "OwnShipLength" ), g_n_ownship_length_meters );
    conf->Write( _T ( "OwnShipWidth" ), g_n_ownship_beam_meters );
    conf->Write( _T ( "OwnShipGPSOffsetX" ), g_n_gps_antenna_offset_x );
    conf->Write( _T ( "OwnShipGPSOffsetY" ), g_n_gps_antenna_offset_y );
    conf->Write( _T ( "OwnShipMinSize" ), g_n_ownship_min_mm );
    conf->Write( _T ( "OwnShipSogCogCalc" ), g_own_ship_sog_cog_calc );
    conf->Write( _T ( "OwnShipSogCogCalcDampSec"), g_own_ship_sog_cog_calc_damp_sec );
    
    conf->Write( _T ( "RouteArrivalCircleRadius" ), wxString::Format( _T("%.2f"), g_n_arrival_circle_radius ));
    conf->Write( _T ( "ChartQuilting" ), g_bQuiltEnable );
    
    conf->Write( _T ( "StartWithTrackActive" ), g_bTrackCarryOver );
    conf->Write( _T ( "AutomaticDailyTracks" ), g_bTrackDaily );
    conf->Write( _T ( "TrackRotateAt" ), g_track_rotate_time );
    conf->Write( _T ( "TrackRotateTimeType" ), g_track_rotate_time_type );
    conf->Write( _T ( "HighlightTracks" ), g_bHighliteTracks );
    
    conf->Write( _T ( "InitialStackIndex" ), g_restore_stackindex );
    conf->Write( _T ( "InitialdBIndex" ), g_restore_dbindex );
    conf->Write( _T ( "ActiveChartGroup" ), g_GroupIndex );
    
    conf->Write( _T ( "AnchorWatch1GUID" ), g_AW1GUID );
    conf->Write( _T ( "AnchorWatch2GUID" ), g_AW2GUID );
    
    conf->Write( _T ( "ToolbarX" ), g_maintoolbar_x );
    conf->Write( _T ( "ToolbarY" ), g_maintoolbar_y );
    conf->Write( _T ( "ToolbarOrient" ), g_maintoolbar_orient );
    
    conf->Write( _T ( "iENCToolbarX" ), g_iENCToolbarPosX );
    conf->Write( _T ( "iENCToolbarY" ), g_iENCToolbarPosY );
    
    if ( !g_bInlandEcdis ){  
        conf->Write( _T ( "ToolbarConfig" ), g_toolbarConfig );
        conf->Write( _T ( "DistanceFormat" ), g_iDistanceFormat );
        conf->Write( _T ( "SpeedFormat" ), g_iSpeedFormat );
        conf->Write( _T ( "ShowDepthUnits" ), g_bShowDepthUnits );
    }

    conf->Write( _T ( "MobileTouch" ), g_btouch );
    conf->Write( _T ( "ResponsiveGraphics" ), g_bresponsive );
    
    conf->Write( _T ( "AutoHideToolbar" ), g_bAutoHideToolbar );
    conf->Write( _T ( "AutoHideToolbarSecs" ), g_nAutoHideToolbar );
    
    conf->Write( _T ( "DisplaySizeMM" ), g_config_display_size_mm );
    conf->Write( _T ( "DisplaySizeManual" ), g_config_display_size_manual );
    
    conf->Write( _T ( "PlanSpeed" ), wxString::Format( _T("%.2f"), g_PlanSpeed ));
    
    wxString vis, invis;
    LayerList::iterator it;
    int index = 0;
    for( it = ( *pLayerList ).begin(); it != ( *pLayerList ).end(); ++it, ++index ) {
        Layer *lay = (Layer *) ( *it );
        if( lay->IsVisibleOnChart() ) vis += ( lay->m_LayerName ) + _T(";");
        else
            invis += ( lay->m_LayerName ) + _T(";");
    }
    conf->Write( _T ( "VisibleLayers" ), vis );
    conf->Write( _T ( "InvisibleLayers" ), invis );
    
    conf->Write( _T ( "Locale" ), g_locale );
    conf->Write( _T ( "LocaleOverride" ), g_localeOverride );
    
    
    // LIVE ETA OPTION
    conf->Write( _T( "LiveETA" ), g_bShowLiveETA);
    conf->Write( _T( "DefaultBoatSpeed" ), g_defaultBoatSpeed);
    
    //    S57 Object Filter Settings
    conf->SetPath( _T ( "/Settings/ObjectFilter" ) );
    
    #ifdef USE_S57
    if( ps52plib ) {
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            
            wxString st1( _T ( "viz" ) );
            char name[7];
            strncpy( name, pOLE->OBJLName, 6 );
            name[6] = 0;
            st1.Append( wxString( name, wxConvUTF8 ) );
            conf->Write( st1, pOLE->nViz );
        }
    }
    #endif
    
    //    Global State
    
    conf->SetPath( _T ( "/Settings/GlobalState" ) );
    
    //    Various Options
    conf->SetPath( _T ( "/Settings/GlobalState" ) );
    if ( !g_bInlandEcdis )
        conf->Write( _T ( "nColorScheme" ), (int) gFrame->GetColorScheme() );
    
    
    //    AIS
    conf->SetPath( _T ( "/Settings/AIS" ) );
    
    conf->Write( _T ( "bNoCPAMax" ), g_bCPAMax );
    conf->Write( _T ( "NoCPAMaxNMi" ), g_CPAMax_NM );
    conf->Write( _T ( "bCPAWarn" ), g_bCPAWarn );
    conf->Write( _T ( "CPAWarnNMi" ), g_CPAWarn_NM );
    conf->Write( _T ( "bTCPAMax" ), g_bTCPA_Max );
    conf->Write( _T ( "TCPAMaxMinutes" ), g_TCPA_Max );
    conf->Write( _T ( "bMarkLostTargets" ), g_bMarkLost );
    conf->Write( _T ( "MarkLost_Minutes" ), g_MarkLost_Mins );
    conf->Write( _T ( "bRemoveLostTargets" ), g_bRemoveLost );
    conf->Write( _T ( "RemoveLost_Minutes" ), g_RemoveLost_Mins );
    conf->Write( _T ( "bShowCOGArrows" ), g_bShowCOG );
    conf->Write( _T ( "CogArrowMinutes" ), g_ShowCOG_Mins );
    conf->Write( _T ( "bShowTargetTracks" ), g_bAISShowTracks );
    conf->Write( _T ( "TargetTracksMinutes" ), g_AISShowTracks_Mins );
    
    conf->Write( _T ( "bHideMooredTargets" ), g_bHideMoored );
    conf->Write( _T ( "MooredTargetMaxSpeedKnots" ), g_ShowMoored_Kts );
    
    conf->Write( _T ( "bAISAlertDialog" ), g_bAIS_CPA_Alert );
    conf->Write( _T ( "bAISAlertAudio" ), g_bAIS_CPA_Alert_Audio );
    conf->Write( _T ( "AISAlertAudioFile" ), g_sAIS_Alert_Sound_File );
    conf->Write( _T ( "bAISAlertSuppressMoored" ), g_bAIS_CPA_Alert_Suppress_Moored );
    conf->Write( _T ( "bShowAreaNotices" ), g_bShowAreaNotices );
    conf->Write( _T ( "bDrawAISSize" ), g_bDrawAISSize );
    conf->Write( _T ( "bShowAISName" ), g_bShowAISName );
    conf->Write( _T ( "ShowAISTargetNameScale" ), g_Show_Target_Name_Scale );
    conf->Write( _T ( "bWplIsAprsPositionReport" ), g_bWplIsAprsPosition );
    conf->Write( _T ( "AISCOGPredictorWidth" ), g_ais_cog_predictor_width );
    conf->Write( _T ( "bShowScaledTargets" ), g_bAllowShowScaled );
    conf->Write( _T ( "AISScaledNumber" ), g_ShowScaled_Num );    
    conf->Write( _T ( "AISScaledNumberWeightSOG" ), g_ScaledNumWeightSOG );
    conf->Write( _T ( "AISScaledNumberWeightCPA" ), g_ScaledNumWeightCPA );
    conf->Write( _T ( "AISScaledNumberWeightTCPA" ), g_ScaledNumWeightTCPA );
    conf->Write( _T ( "AISScaledNumberWeightRange" ), g_ScaledNumWeightRange );
    conf->Write( _T ( "AISScaledNumberWeightSizeOfTarget" ), g_ScaledNumWeightSizeOfT ); 
    conf->Write( _T ( "AISScaledSizeMinimal" ), g_ScaledSizeMinimal );
    conf->Write( _T ( "AISShowScaled"), g_bShowScaled);
    
    conf->Write( _T ( "AlertDialogSizeX" ), g_ais_alert_dialog_sx );
    conf->Write( _T ( "AlertDialogSizeY" ), g_ais_alert_dialog_sy );
    conf->Write( _T ( "AlertDialogPosX" ), g_ais_alert_dialog_x );
    conf->Write( _T ( "AlertDialogPosY" ), g_ais_alert_dialog_y );
    conf->Write( _T ( "QueryDialogPosX" ), g_ais_query_dialog_x );
    conf->Write( _T ( "QueryDialogPosY" ), g_ais_query_dialog_y );
    conf->Write( _T ( "AISTargetListPerspective" ), g_AisTargetList_perspective );
    conf->Write( _T ( "AISTargetListRange" ), g_AisTargetList_range );
    conf->Write( _T ( "AISTargetListSortColumn" ), g_AisTargetList_sortColumn );
    conf->Write( _T ( "bAISTargetListSortReverse" ), g_bAisTargetList_sortReverse );
    conf->Write( _T ( "AISTargetListColumnSpec" ), g_AisTargetList_column_spec );
    conf->Write( _T ( "S57QueryDialogSizeX" ), g_S57_dialog_sx );
    conf->Write( _T ( "S57QueryDialogSizeY" ), g_S57_dialog_sy );
    conf->Write( _T ( "bAISRolloverShowClass" ), g_bAISRolloverShowClass );
    conf->Write( _T ( "bAISRolloverShowCOG" ), g_bAISRolloverShowCOG );
    conf->Write( _T ( "bAISRolloverShowCPA" ), g_bAISRolloverShowCPA );
    conf->Write( _T ( "bAISAlertAckTimeout" ), g_bAIS_ACK_Timeout );
    conf->Write( _T ( "AlertAckTimeoutMinutes" ), g_AckTimeout_Mins );
    
    #ifdef USE_S57
    conf->SetPath( _T ( "/Settings/GlobalState" ) );
    if( ps52plib ) {
        conf->Write( _T ( "bShowS57Text" ), ps52plib->GetShowS57Text() );
        conf->Write( _T ( "bShowS57ImportantTextOnly" ), ps52plib->GetShowS57ImportantTextOnly() );
        if ( !g_bInlandEcdis )
            conf->Write( _T ( "nDisplayCategory" ), (long) ps52plib->GetDisplayCategory() );
        conf->Write( _T ( "nSymbolStyle" ), (int) ps52plib->m_nSymbolStyle );
        conf->Write( _T ( "nBoundaryStyle" ), (int) ps52plib->m_nBoundaryStyle );
        
        conf->Write( _T ( "bShowSoundg" ), ps52plib->m_bShowSoundg );
        conf->Write( _T ( "bShowMeta" ), ps52plib->m_bShowMeta );
        conf->Write( _T ( "bUseSCAMIN" ), ps52plib->m_bUseSCAMIN );
        conf->Write( _T ( "bShowAtonText" ), ps52plib->m_bShowAtonText );
        conf->Write( _T ( "bShowLightDescription" ), ps52plib->m_bShowLdisText );
        conf->Write( _T ( "bExtendLightSectors" ), ps52plib->m_bExtendLightSectors );
        conf->Write( _T ( "bDeClutterText" ), ps52plib->m_bDeClutterText );
        conf->Write( _T ( "bShowNationalText" ), ps52plib->m_bShowNationalTexts );
        
        conf->Write( _T ( "S52_MAR_SAFETY_CONTOUR" ), S52_getMarinerParam( S52_MAR_SAFETY_CONTOUR ) );
        conf->Write( _T ( "S52_MAR_SHALLOW_CONTOUR" ), S52_getMarinerParam( S52_MAR_SHALLOW_CONTOUR ) );
        conf->Write( _T ( "S52_MAR_DEEP_CONTOUR" ), S52_getMarinerParam( S52_MAR_DEEP_CONTOUR ) );
        conf->Write( _T ( "S52_MAR_TWO_SHADES" ), S52_getMarinerParam( S52_MAR_TWO_SHADES ) );
        conf->Write( _T ( "S52_DEPTH_UNIT_SHOW" ), ps52plib->m_nDepthUnitDisplay );
    }
    #endif
    
    conf->SetPath( _T ( "/Settings/Others" ) );
    
    // Radar rings
    conf->Write( _T ( "ShowRadarRings" ), (bool)(g_iNavAidRadarRingsNumberVisible > 0) );  //3.0.0 config support
    conf->Write( _T ( "RadarRingsNumberVisible" ), g_iNavAidRadarRingsNumberVisible );
    conf->Write( _T ( "RadarRingsStep" ), g_fNavAidRadarRingsStep );
    conf->Write( _T ( "RadarRingsStepUnits" ), g_pNavAidRadarRingsStepUnits );
    conf->Write( _T ( "RadarRingsColour" ), g_colourOwnshipRangeRingsColour.GetAsString( wxC2S_HTML_SYNTAX ) );
    
    // Waypoint Radar rings
    conf->Write( _T ( "WaypointRangeRingsNumber" ), g_iWaypointRangeRingsNumber );
    conf->Write( _T ( "WaypointRangeRingsStep" ), g_fWaypointRangeRingsStep );
    conf->Write( _T ( "WaypointRangeRingsStepUnits" ), g_iWaypointRangeRingsStepUnits );
    conf->Write( _T ( "WaypointRangeRingsColour" ), g_colourWaypointRangeRingsColour.GetAsString( wxC2S_HTML_SYNTAX ) );
    
    conf->Write( _T ( "ConfirmObjectDeletion" ), g_bConfirmObjectDelete );
    
    // Waypoint dragging with mouse
    conf->Write( _T ( "WaypointPreventDragging" ), g_bWayPointPreventDragging );
    
    conf->Write( _T ( "EnableZoomToCursor" ), g_bEnableZoomToCursor );
    
    conf->Write( _T ( "TrackIntervalSeconds" ), g_TrackIntervalSeconds );
    conf->Write( _T ( "TrackDeltaDistance" ), g_TrackDeltaDistance );
    conf->Write( _T ( "TrackPrecision" ), g_nTrackPrecision );
    
    conf->Write( _T ( "RouteLineWidth" ), g_route_line_width );
    conf->Write( _T ( "TrackLineWidth" ), g_track_line_width );
    conf->Write( _T ( "TrackLineColour" ), g_colourTrackLineColour.GetAsString( wxC2S_HTML_SYNTAX ) );
    conf->Write( _T ( "DefaultWPIcon" ), g_default_wp_icon );
    
    conf->Flush();
    
    delete conf;
    
    return true;
}
    

