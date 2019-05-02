
#pragma once

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>

#include <thread>
#include <mutex>

#include "skynet_cpp/snNet.h"

class MainWin : public wxFrame
{
   
public:
    MainWin(wxApp*);
    ~MainWin();          
  
private:
 
    struct UI{
        wxPanel* pnLeft = nullptr;
        wxScrolledWindow* plot = nullptr;
        wxRichTextCtrl* txtStatusMess = nullptr;       
    };
    UI ui;
    
    wxRect selRect_;
    bool isLeftTop_ = false,
         isLeftBottom_ = false,
         isRightTop_ = false,
         isRightBottom_ = false,
         isCentr_ = false;

    std::string cname_;
    wxImage cimg_;

    std::thread* pthrRec_ = nullptr;
    std::mutex mtx_;
    bool recStop_ = false;

    SN_API::Net* pSNet_ = nullptr;

    void statusMess(const std::string&);

    void createTools();
    bool createNet();
    void connects();
  

    void onPaint(wxPaintEvent&); 
    void recImage();
    std::vector<std::string> getObjNames();
   
};
