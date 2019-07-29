
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/splitter.h>

#include <thread>
#include <fstream>

#include "mainWin.h"
#include "skynet_cpp/snNet.h"
#include "rapidjson/document.h"

using namespace std;

// %H:%M:%S:%MS
string CurrTimeMs() {

    time_t ct = time(nullptr);
    tm* lct = localtime(&ct);

    uint64_t ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    uint64_t mspr = ms / 1000;
    ms -= mspr * 1000;

    char curDate[32];
    strftime(curDate, 32, "%H:%M:%S:", lct);

    sprintf(curDate, "%s%03d", curDate, int(ms));

    return curDate;
}

void MainWin::statusMess(const std::string& mess){
  
    ui.txtStatusMess->SetCaretPosition(-1);
    ui.txtStatusMess->WriteText(CurrTimeMs() + " " + mess + "\n");
}

void MainWin::createTools(){
    
    wxToolBar* toolBar = this->CreateToolBar();

    toolBar->AddSeparator();

    wxButton* btn = new wxButton(toolBar, wxID_ANY, wxT("Image"), wxDefaultPosition, wxSize(60, 35));
    toolBar->SetToolBitmapSize(btn->GetSize());

    toolBar->AddControl(btn);
    toolBar->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this](wxCommandEvent& evt){

        wxString wildcard = wxT("Image files (*.jpg,*.png,*.bmp)|*.jpg;*.png;*.bmp");
        wxFileDialog openFileDialog(this, _("Open Image file"), "", "",
            wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        
        if (openFileDialog.ShowModal() == wxID_CANCEL)
            return;     

        cimg_ = wxImage(openFileDialog.GetPath());

        Refresh();

    }, wxID_ANY);

    toolBar->Realize();

    ////////////////////////////////

    this->SetBackgroundColour(wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_BTNFACE));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizerAndFit(mainSizer);

    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this);

    mainSplitter->SetSize(GetClientSize());
    mainSplitter->SetSashGravity(0.15);

    ui.pnLeft = new wxPanel(mainSplitter);

    ui.pnLeft->SetWindowStyle(wxBORDER_SIMPLE);
    ui.pnLeft->SetSizerAndFit(new wxBoxSizer(wxVERTICAL));
    ui.pnLeft->SetBackgroundColour(*wxWHITE);

    ui.txtStatusMess = new wxRichTextCtrl(ui.pnLeft, wxID_ANY);
    ui.txtStatusMess->SetWindowStyle(wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE);
    ui.txtStatusMess->SetEditable(false);

    ui.pnLeft->GetSizer()->Add(ui.txtStatusMess, wxSizerFlags(1).Expand());

    ////////////////////////////////

    ui.plot = new wxScrolledWindow(mainSplitter);
    ui.plot->SetWindowStyle(wxBORDER_SIMPLE);

    ////////////////////////////////

    mainSplitter->SplitVertically(ui.pnLeft, ui.plot);

    mainSizer->Add(mainSplitter, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 5));
    

    ////////////////////////////////

    CreateStatusBar();
    SetStatusText("");

}

void MainWin::connects(){

    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt){

        if (pthrRec_){
            recStop_ = true;
            pthrRec_->join();
        }

        exit(0);

    }, wxID_ANY);

    ui.plot->Bind(wxEVT_PAINT, &MainWin::onPaint, this);
    ui.plot->Bind(wxEVT_ERASE_BACKGROUND, [this](wxEraseEvent&){}, wxID_ANY);
    ui.plot->Bind(wxEVT_SIZE, [this](wxSizeEvent&){
               
        int cwidth = ui.plot->GetSize().GetWidth(),
            cheight = ui.plot->GetSize().GetHeight();
        
        std::lock_guard<std::mutex> lck(mtx_);

        ui.plot->Refresh();
    }, wxID_ANY);
    ui.plot->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& evt){
                
        auto cp = evt.GetPosition();

        isLeftTop_ = (abs(cp.x - selRect_.GetLeftTop().x) < 10) && (abs(cp.y - selRect_.GetLeftTop().y) < 10);
        isLeftBottom_ = (abs(cp.x - selRect_.GetLeftBottom().x) < 10) && (abs(cp.y - selRect_.GetLeftBottom().y) < 10);
        isRightTop_ = (abs(cp.x - selRect_.GetRightTop().x) < 10) && (abs(cp.y - selRect_.GetRightTop().y) < 10);
        isRightBottom_ = (abs(cp.x - selRect_.GetRightBottom().x) < 10) && (abs(cp.y - selRect_.GetRightBottom().y) < 10);

        isCentr_ = selRect_.Contains(cp);

    }, wxID_ANY);
    ui.plot->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& evt){
                
        isLeftTop_ = false;
        isLeftBottom_ = false;
        isRightTop_ = false;
        isRightBottom_ = false;
        isCentr_ = false;

    }, wxID_ANY);
    ui.plot->Bind(wxEVT_MOTION, [this](wxMouseEvent& evt){

        if (evt.LeftIsDown() && evt.Dragging()) {
                    
            auto cp = evt.GetPosition();
                       
            if (isLeftTop_){
                auto rb = selRect_.GetRightBottom();
                selRect_.SetLeftTop(cp);
                selRect_.SetRightBottom(rb);
            }
            
            else if (isLeftBottom_){
                auto rt = selRect_.GetRightTop();
                selRect_.SetLeftBottom(cp);
                selRect_.SetRightTop(rt);
            }
            
            else if (isRightTop_){
                auto lb = selRect_.GetLeftBottom();
                selRect_.SetRightTop(cp);
                selRect_.SetLeftBottom(lb);
            }

            else if (isRightBottom_)
                selRect_.SetRightBottom(cp);
            
            else if (isCentr_){
                selRect_.SetPosition(wxPoint(cp.x - selRect_.GetWidth() / 2, cp.y - selRect_.GetHeight() / 2));

                int w = ui.plot->GetSize().GetWidth(),
                    h = ui.plot->GetSize().GetHeight();
                
                int x = selRect_.GetLeftTop().x,
                    y = selRect_.GetLeftTop().y;

                x = max<int>(min<int>(x, w - 1), 0);
                y = max<int>(min<int>(y, h - 1), 0);

                selRect_.SetLeftTop(wxPoint(x, y));

                x = selRect_.GetRightBottom().x;
                y = selRect_.GetRightBottom().y;

                x = max<int>(min<int>(x, w), 0);
                y = max<int>(min<int>(y, h), 0);

                selRect_.SetRightBottom(wxPoint(x, y));
            }

            std::lock_guard<std::mutex> lck(mtx_);
                       
            ui.plot->Refresh();
        }   

    }, wxID_ANY);
   
}

bool MainWin::createNet(){

    string archPath = "resNet50Struct.json",
           weightPath = "resNet50Weights.dat";

    std::ifstream ifs;
    ifs.open(archPath, std::ifstream::in);

    if (!ifs.good()){
        statusMess("error open file : " + archPath);
        return false;
    }

    ifs.seekg(0, ifs.end);
    size_t length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    string jnArch; jnArch.resize(length);
    ifs.read((char*)jnArch.data(), length);

    pSNet_ = new SN_API::Net(jnArch, weightPath);

    string err = pSNet_->getLastErrorStr();
    statusMess(err);

    return err.empty();
}

MainWin::MainWin(wxApp* app) :
    wxFrame(NULL, wxID_ANY, wxT("resnetDemo"), wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE){
       
    createTools();  
   
    connects();

    if (!createNet())
        return;

    cimg_ = wxImage("defaultImage.jpg");

    selRect_ = wxRect(50, 50, 100, 100);

    pthrRec_ = new std::thread([](MainWin* win){ win->recImage(); }, this);

    this->Maximize(true);
}

MainWin::~MainWin(){

    
}

vector<string> MainWin::getObjNames(){

    namespace rj = rapidjson;

    std::ifstream ifs;
    ifs.open("imagenet_class_index.json", std::ifstream::in);

    vector<string> res(1000, "nobody");

    rj::Document doc;
    if (ifs.good()){

        ifs.seekg(0, ifs.end);
        size_t length = ifs.tellg();
        ifs.seekg(0, ifs.beg);

        string jnArch; jnArch.resize(length);
        ifs.read((char*)jnArch.data(), length);

        doc.Parse(jnArch.c_str());

        if (!doc.IsObject()){
            statusMess("'imagenet_class_index.json' !jnDoc.IsObject()");
            return res;
        }

        for (int i = 0; i < 1000; ++i)
            res[i] = doc[to_string(i).c_str()].GetArray()[1].GetString();
    }
    else
        statusMess("not found file 'imagenet_class_index.json'");

    return res;
}

void MainWin::recImage(){

    namespace sn = SN_API;
    
    auto objNames = getObjNames();

    int classCnt = 1000, w = 224, h = 224;

    sn::Tensor inLayer(sn::snLSize(w, h, 3, 1));
    sn::Tensor outLayer(sn::snLSize(classCnt, 1, 1, 1));

    while (!recStop_){
    
        
        double scaleW = ui.plot->GetSize().GetWidth() / double(cimg_.GetWidth()),
               scaleH = ui.plot->GetSize().GetHeight() / double(cimg_.GetHeight());
               
        wxRect rct(selRect_.x / scaleW, selRect_.y / scaleH, selRect_.width / scaleW, selRect_.height / scaleH);

        auto img = cimg_.GetSubImage(rct).Copy();
       
        if (!img.IsOk()) continue;

        img.Rescale(w, h);
               
        float* refData = inLayer.data();
        auto pt = img.GetData();
               
        for (size_t r = 0; r < h; ++r){

            for (size_t c = 0; c < w; ++c){
                refData[c + r * w] = pt[c * 3 + 2 + r * w * 3];         // B
                refData[c + r * w + h * w] = pt[c * 3 + 1 + r * w * 3]; // G
                refData[c + r * w + h * w * 2] = pt[c * 3 + r * w * 3]; // R
            }
        }
    
        pSNet_->forward(false, inLayer, outLayer);

        float* refOutput = outLayer.data();

        int maxInx = distance(refOutput, max_element(refOutput, refOutput + classCnt));
            
        statusMess(objNames[maxInx] + " accurate " + to_string(refOutput[maxInx]));

        if (cname_ != objNames[maxInx]){
            cname_ = objNames[maxInx];

            std::lock_guard<std::mutex> lck(mtx_);

            ui.plot->Refresh();
        }
    }    
}

void MainWin::onPaint(wxPaintEvent& evt){

    wxPaintDC dc(ui.plot);
      
    if (!cimg_.IsOk()) return;

    double scaleW = ui.plot->GetSize().GetWidth() / double(cimg_.GetWidth()),
           scaleH = ui.plot->GetSize().GetHeight() / double(cimg_.GetHeight());
      
    dc.SetUserScale(scaleW, scaleH);

    dc.DrawBitmap(cimg_, wxPoint(0, 0));
  
    dc.SetUserScale(1, 1);
        
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.DrawText(cname_, wxPoint(selRect_.GetLeftTop().x + 5, selRect_.GetLeftTop().y - 25));

    dc.SetPen(wxPen(wxColourDatabase().Find("Green"), 2));

    dc.DrawLine(selRect_.GetLeftTop(), selRect_.GetLeftBottom());
    dc.DrawLine(selRect_.GetLeftTop(), selRect_.GetRightTop());
    dc.DrawLine(selRect_.GetRightTop(), selRect_.GetRightBottom());
    dc.DrawLine(selRect_.GetLeftBottom(), selRect_.GetRightBottom());

    dc.SetBrush(wxBrush(wxColourDatabase().Find("Green")));

    dc.DrawCircle(selRect_.GetLeftTop(), 5);
    dc.DrawCircle(selRect_.GetLeftBottom(), 5);
    dc.DrawCircle(selRect_.GetRightTop(), 5);
    dc.DrawCircle(selRect_.GetRightBottom(), 5);
}
