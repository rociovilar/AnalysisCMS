// Constants
//------------------------------------------------------------------------------

enum {
  ee,
  mm,
  em,
  ll,
  nchannel  // This line should be always last
};

const TString schannel[nchannel] = {
  "ee",
  "mm",
  "em",
  "ll"
};

const TString lchannel[nchannel] = {
  "ee",
  "#mu#mu",
  "e#mu",
  "ll"
};

// Constants
//------------------------------------------------------------------------------
const float   zmin =  76;  // [GeV]
const float   zmax = 106;  // [GeV]

//const int     nmetcut = 7;
const int     nmetcut = 12;

const float   metcut [nmetcut] = {-1, 10, 20, 30, 40, 45, 50, 60, 70,  80, 90,  -1};  // [GeV]
//const float   metcut [nmetcut] = {-1, 10, 20, 30, 40, 45, 50, 60, 70, 80, 90, -1};  // [GeV]
const float   metdraw[nmetcut] = { 0, 10, 20, 30, 40, 45, 50, 60, 70, 80, 90, 100};  // [GeV]
//const float   metdraw[nmetcut] = { 0, 10, 20, 30, 40, 45, 100};  // [GeV]

const bool    includeVZ            = true;
const bool    printResults         = true;
const bool    R_study              = true; 
const bool    show_R_study_Results = true; 

const double  lumi_fb  = 2.318;

const TString outputdir = "figures";


// Functions
//------------------------------------------------------------------------------
void     GetScale  (int           ch,
		    float&        scale_value,
		    float&        scale_error,
		    float&        R_data_value,
		    float&        R_data_error,
		    float&        R_dy_value,
		    float&        R_dy_error);

float    errAB2    (float         a,
		    float         err_a,
		    float         b,
		    float         err_b);

float    errRatio  (float         a,
		    float         err_a,
		    float         b,
		    float         err_b);

void     SetGraph  (TGraphErrors* g,
		    Color_t       color,
		    Style_t       style);

TLegend* DrawLegend(Float_t       x1,
		    Float_t       y1,
		    TObject*      hist,
		    TString       label,
		    TString       option  = "lp",
		    Float_t       tsize   = 0.030,
		    Float_t       xoffset = 0.200,
		    Float_t       yoffset = 0.050);

void     DrawLatex (Font_t        tfont,
		    Float_t       x,
		    Float_t       y,
		    Float_t       tsize,
		    Short_t       align,
		    const char*   text,
		    Bool_t        setndc = true);


// Data members
//------------------------------------------------------------------------------
TH2D*        h2_data[ll];  // ee, mm, em
TH2D*        h2_dy  [ll];  // ee, mm, em
TH2D*        h2_wz  [ll];  // ee, mm, em
TH2D*        h2_zz  [ll];  // ee, mm, em

TCanvas*     canvas[3];    // R_ee, R_mm, scale
TMultiGraph* mgraph[3];    // R_ee, R_mm, scale
TCanvas*     R_c   [3];    // R_dy_* ; R_**_ee; R_**_mm; *=> ee, mm; ** zz, wz, dy  
TMultiGraph* Rgraph[3];    // R_dy_* ; R_**_ee; R_**_mm; *=> ee, mm; ** zz, wz, dy
      
float R_zz_value[nmetcut][2],   R_zz_error[nmetcut][2], R_wz_value[nmetcut][2],   R_wz_error[nmetcut][2];     

float        k_value[2];   // ee, mm
float        k_error[2];   // ee, mm

int          bin_zmin;
int          bin_zmax;
int          bin_metmin;
int          bin_metmax;
int          cutt; 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// This macro computes the DY scale factor for analyses that veto the Z-peak. It
// needs as input a set of two-dimensional histograms, with MET in the x-axis
// and m2l in the y-axis. These histograms have been filled once all analysis
// cuts have been applied, but removing the Z-peak veto and any MET requirement.
// The scale factor for the ee channel (the procedure is identical for the mm
// channel) is computed with equations (1) and (2).
//
//    (1) k_ee  = 0.5 * sqrt(n_ee / n_mm);
//    (2) scale = (n_in_ee - n_in_wz - n_in_zz - k_ee * n_in_em) / n_in_dy;
//
// Results for MET > 40 GeV and 2.318 fb-1,
//
//    ee SF(est/DY) = 1.284 +- 0.117
//    mm SF(est/DY) = 1.225 +- 0.079
//
// Results for MET > 40 GeV and 2.318 fb-1
// when the peaking backgrounds are not considered,
//
//    ee SF(est/DY)  1.305 +- 0.118
//    mm SF(est/DY)  1.242 +- 0.079
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void getDYScaleV2(TString analysis = "Top")
{
  gInterpreter->ExecuteMacro("../test/PaperStyle.C");

  gSystem->mkdir(outputdir, kTRUE);

  TFile* file_data = new TFile("../rootfiles/nominal/" + analysis + "/01_Data.root",     "read");
  TFile* file_dy   = new TFile("../rootfiles/nominal/" + analysis + "/07_ZJets.root",    "read");
  TFile* file_wz   = new TFile("../rootfiles/nominal/" + analysis + "/02_WZTo3LNu.root", "read");
  TFile* file_zz   = new TFile("../rootfiles/nominal/" + analysis + "/03_ZZ.root",       "read");


  // Get MET (x-axis) vs m2l (y-axis) TH2D histograms
  //----------------------------------------------------------------------------
  for (int i=ee; i<ll; i++)
    {
      h2_data[i] = (TH2D*)file_data->Get(analysis + "/02_Routin/h_metPfType1_m2l_" + schannel[i]);
      h2_dy  [i] = (TH2D*)file_dy  ->Get(analysis + "/02_Routin/h_metPfType1_m2l_" + schannel[i]);
      h2_wz  [i] = (TH2D*)file_wz  ->Get(analysis + "/02_Routin/h_metPfType1_m2l_" + schannel[i]);
      h2_zz  [i] = (TH2D*)file_zz  ->Get(analysis + "/02_Routin/h_metPfType1_m2l_" + schannel[i]);

      h2_dy[i]->Scale(lumi_fb);
      h2_wz[i]->Scale(lumi_fb);
      h2_zz[i]->Scale(lumi_fb);

    }


  // Compute k_ee and k_mm from m2l without any MET cut
  //----------------------------------------------------------------------------
  TH1D* h_m2l_ee = (TH1D*)h2_data[ee]->ProjectionY("h_m2l_ee");
  TH1D* h_m2l_mm = (TH1D*)h2_data[mm]->ProjectionY("h_m2l_mm");

  bin_zmin = h_m2l_ee->FindBin(zmin);
  bin_zmax = h_m2l_ee->FindBin(zmax)-1;

  float n_ee = h_m2l_ee->Integral(bin_zmin, bin_zmax);
  float n_mm = h_m2l_mm->Integral(bin_zmin, bin_zmax);

  k_value[ee] = 0.5 * sqrt(n_ee / n_mm);
  k_value[mm] = 0.5 * sqrt(n_mm / n_ee);

  k_error[ee] = sqrt((1 + n_ee/n_mm) / n_mm) / 4.0;
  k_error[mm] = sqrt((1 + n_mm/n_ee) / n_ee) / 4.0;


  // Do the work
  //----------------------------------------------------------------------------
  TGraphErrors* graph_R_data[2];
  TGraphErrors* graph_R_dy  [2];
  TGraphErrors* graph_scale [2];

 // if (R_study)
 // {
   TGraphErrors* graph_R_zz  [2];
   TGraphErrors* graph_R_wz  [2];

   //for (int k=ee; k<=mm; k++)
   // {
    //graph_R_zz [k] = new TGraphErrors();
    //graph_R_wz [k] = new TGraphErrors();
   // }
 // }


  for (int k=ee; k<=mm; k++)
    {
      graph_R_data[k] = new TGraphErrors();
      graph_R_dy  [k] = new TGraphErrors();
      graph_scale [k] = new TGraphErrors();

      graph_R_zz [k] = new TGraphErrors();
      graph_R_wz [k] = new TGraphErrors();
    }

  for (int j=0; j<nmetcut-1; j++)
    {
      bin_metmin = (metcut[j]   > 0) ? h2_data[ee]->GetXaxis()->FindBin(metcut[j])     : -1;
      bin_metmax = (metcut[j+1] > 0) ? h2_data[ee]->GetXaxis()->FindBin(metcut[j+1])-1 : -1;

      if (printResults)
	{
	  printf("\n--------------------------------------------------------\n");
	  printf("\n %.0f < MET < %.0f GeV\n", metcut[j], metcut[j+1]);
	}

      float scale[2], scale_err[2], R_data[2], R_data_err[2], R_dy[2], R_dy_err[2];
 //     float R_zz_value[2],   R_zz_error[2], R_wz_value[2],   R_wz_error[2];     
      cutt = j; 
 
      GetScale(ee, scale[ee], scale_err[ee], R_data[ee], R_data_err[ee], R_dy[ee], R_dy_err[ee]);
      GetScale(mm, scale[mm], scale_err[mm], R_data[mm], R_data_err[mm], R_dy[mm], R_dy_err[mm]);

      for (int k=ee; k<=mm; k++)
	{
	  
      
          graph_R_data[k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), R_data[k]);
	  graph_R_dy  [k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), R_dy  [k]);
	  graph_scale [k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), scale [k]);
          graph_R_zz  [k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), R_zz_value  [j][k]);
          graph_R_wz  [k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), R_wz_value  [j][k]);

	  std::cout << "___________________________duda _____________________________" << std::endl;
          std::cout << schannel[k] << "\t j " << j << std::endl;
          std::cout << "R_dy" << R_dy[k] << "\t  R_dy_err " << R_dy_err[k] << std::endl;
          std::cout << "___________________________duda _____________________________" << std::endl;


	  graph_R_data[k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), R_data_err[k]);
	  graph_R_dy  [k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), R_dy_err  [k]);
	  graph_scale [k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), scale_err [k]);
          graph_R_zz  [k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), R_zz_error  [j][k]);
          graph_R_wz  [k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), R_wz_error  [j][k]);
	}
	
    // if (R_study)
    //   {
//	float R_zz_value[2],   R_zz_error[2], R_wz_value[2],   R_wz_error[2];
//
//	for (int k=ee; k<=mm; k++)
//        {
//	  graph_R_zz  [k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), R_zz_value  [k]);
//	  graph_R_wz  [k]->SetPoint(j, 0.5* (metdraw[j+1] + metdraw[j]), R_wz_value  [k]);
//         
//          graph_R_zz  [k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), R_zz_error  [k]);
//          graph_R_wz  [k]->SetPointError(j, 0.5* (metdraw[j+1] - metdraw[j]), R_wz_error  [k]);
//        }
      // }

    }


  // Cosmetics
  //----------------------------------------------------------------------------
  SetGraph(graph_R_data[ee], kBlack, kFullCircle);
  SetGraph(graph_R_data[mm], kBlack, kFullCircle);
  SetGraph(graph_R_dy[ee],   kRed+1, kOpenSquare);
  SetGraph(graph_R_dy[mm],   kRed+1, kOpenSquare);
  SetGraph(graph_scale[ee],  kBlack, kOpenSquare);
  SetGraph(graph_scale[mm],  kRed+1, kFullCircle);

  //  R study
  // ---------------------------------------------------------------------------

  if (R_study)
  {// (Draw R study)


   // -------------------------------------------------------------------------
   // R_dy_ee, R_dy_mm graph
   // -------------------------------------------------------------------------
  
   // cosmetics;

   SetGraph (graph_R_dy  [ee], kBlue+2, kOpenCircle);
   SetGraph (graph_R_dy  [mm], kRed+2, kFullCircle);

   // draw; 

   R_c[2] = new TCanvas("R out/in_DY", "R out/in_DY");
   Rgraph[2] = new TMultiGraph();

   
   Rgraph[2] -> Add (graph_R_dy[ee]);
   Rgraph[2]-> Add (graph_R_dy[mm]);

   // Make the fit
   //----------------------------------------
   TF1 *ff[2];
   ff[ee]= new TF1("ff_ee", "pol0",0.,100.);
   ff[mm]= new TF1("ff_mm", "pol0",0.,100.);

   graph_R_dy[ee] -> Fit (ff[ee],"rwln");
   std::cout << "prob = " << ff[ee]->GetProb() << "\t chi2 = " << ff[ee]-> GetChisquare() << " \t NDF = " << ff[ee] -> GetNDF() << std::endl;  

   graph_R_dy[mm] -> Fit (ff[mm],"rwln"); 
   std::cout << "prob = " << ff[mm]->GetProb() << "\t chi2 = " << ff[mm]-> GetChisquare() << " \t NDF = " << ff[mm] -> GetNDF() << std::endl;
   //-----------------------------------------
   

   Rgraph[2]->Draw("ap");

   // Plot the Fit
   // ---------------------------------------
   ff[mm]->Draw("same");
   ff[ee]->Draw("same");
  
   ff[ee]->SetLineWidth(1);
   ff[ee]->SetLineColor(kBlue);

   ff[mm]->SetLineWidth(1);
   ff[mm]->SetLineColor(kRed);
   //-----------------------------------------

   Rgraph[2]->GetXaxis()->SetTitleOffset(1.5);
   Rgraph[2]->GetYaxis()->SetTitleOffset(1.7);
   Rgraph[2]->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
   Rgraph[2]->GetYaxis()->SetTitle("R^{out/in}_{DY} = N^{out} / N^{in}");
 
   //Rgraph[2]->SetMinimum(0.);
  // Rgraph[2]->SetMaximum(0.4);

   DrawLegend(0.22, 0.83, (TObject*)graph_R_dy[ee], " ee DY");
   DrawLegend(0.22, 0.77, (TObject*)graph_R_dy  [mm], "mm  DY");

   DrawLatex(42, 0.940, 0.945, 0.050, 31, Form("%.3f fb^{-1} (13TeV)", lumi_fb));

   R_c[2]->Modified();
   R_c[2]->Update();
   R_c[2]->SaveAs(outputdir + "/MC_Routin_DY.png");

   //----------------------------------------------------------------------------
   // R_zz_* , R_wz_*, R_dy_* graph
   // ---------------------------------------------------------------------------

    //cosmetics;
   
    for (int n = ee; n <= mm; n++)
    {
    SetGraph(graph_R_zz[n],   kBlue+2, kFullSquare);
    SetGraph(graph_R_wz[n],   kMagenta+2, kFullCross);
    SetGraph (graph_R_dy[n], kBlack, kFullCircle);
    }
  
    // Draw;

    for (int m=0; m<=1; m++)
    {
    R_c[m] = new TCanvas("R out/in_MC"+ schannel[m], "R out/in_MC study");
    Rgraph[m] = new TMultiGraph();
    Rgraph[m]-> Add (graph_R_dy  [m]);
    Rgraph[m]-> Add (graph_R_zz  [m]);
    Rgraph[m]-> Add (graph_R_wz  [m]);

    Rgraph[m]->Draw("ap");
   
    Rgraph[m]->GetXaxis()->SetTitleOffset(1.5);
    Rgraph[m]->GetYaxis()->SetTitleOffset(1.7);
    Rgraph[m]->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
    Rgraph[m]->GetYaxis()->SetTitle("R^{out/in}_{MC} = N^{out} / N^{in}");

    //Rgraph[m]->SetMinimum(0.);
    //Rgraph[m]->SetMaximum(0.45);

    DrawLegend(0.80, 0.77, (TObject*)graph_R_dy[m], schannel[m] + " DY");
    DrawLegend(0.80, 0.83, (TObject*)graph_R_zz[m], schannel[m] + " ZZ");
    DrawLegend(0.80, 0.77, (TObject*)graph_R_wz[m], schannel[m] + " WZ");

    DrawLatex(42, 0.940, 0.945, 0.050, 31, Form("%.3f fb^{-1} (13TeV)", lumi_fb));

    R_c[m]->Modified();
    R_c[m]->Update();
    R_c[m]->SaveAs(outputdir + "/MC_Routin_" + schannel[m] + ".png");

   } 

  }


  // Cosmetics
  //----------------------------------------------------------------------------
  SetGraph(graph_R_data[ee], kBlack, kFullCircle);
  SetGraph(graph_R_data[mm], kBlack, kFullCircle);
  SetGraph(graph_R_dy[ee],   kRed+1, kOpenSquare);
  SetGraph(graph_R_dy[mm],   kRed+1, kOpenSquare);
  SetGraph(graph_scale[ee],  kBlack, kOpenSquare);
  SetGraph(graph_scale[mm],  kRed+1, kFullCircle);

  // Draw R
  //----------------------------------------------------------------------------
  for (int k=ee; k<=mm; k++)
    {
      canvas[k] = new TCanvas("R out/in " + schannel[k], "R out/in " + schannel[k]);

      mgraph[k] = new TMultiGraph();

      mgraph[k]->Add(graph_R_data[k]);
      mgraph[k]->Add(graph_R_dy  [k]);

      mgraph[k]->Draw("ap");

      mgraph[k]->GetXaxis()->SetTitleOffset(1.5);
      mgraph[k]->GetYaxis()->SetTitleOffset(1.7);
      mgraph[k]->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
      mgraph[k]->GetYaxis()->SetTitle("R^{out/in} = N^{out} / N^{in}");

      mgraph[k]->SetMinimum(-0.01);
      mgraph[k]->SetMaximum(+0.4);

      DrawLegend(0.22, 0.83, (TObject*)graph_R_data[k], " " + lchannel[k] + " estimated (data)");
      DrawLegend(0.22, 0.77, (TObject*)graph_R_dy  [k], " " + lchannel[k] + " DY");

      DrawLatex(42, 0.940, 0.945, 0.050, 31, Form("%.3f fb^{-1} (13TeV)", lumi_fb));

      canvas[k]->Modified();
      canvas[k]->Update();
      canvas[k]->SaveAs(outputdir + "/dy_Routin_" + schannel[k] + ".png");
    }


  // Draw scale
  //----------------------------------------------------------------------------
  canvas[2] = new TCanvas("scale", "est/DY scale");

  mgraph[2] = new TMultiGraph();

  mgraph[2]->Add(graph_scale[ee]);
  mgraph[2]->Add(graph_scale[mm]);

  mgraph[2]->Draw("ap");

  canvas[2]->Update();

  TLine* line = new TLine(canvas[2]->GetUxmin(), 1.0, canvas[2]->GetUxmax(), 1.0);
  
  line->SetLineWidth(2);
  line->SetLineStyle(3);
  line->Draw("same");

  mgraph[2]->GetXaxis()->SetTitleOffset(1.5);
  mgraph[2]->GetYaxis()->SetTitleOffset(1.7);
  mgraph[2]->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
  mgraph[2]->GetYaxis()->SetTitle("scale factor = N^{in}_{est} / N^{in}_{DY}");

  DrawLegend(0.22, 0.83, (TObject*)graph_scale[ee], " " + lchannel[ee]);
  DrawLegend(0.22, 0.77, (TObject*)graph_scale[mm], " " + lchannel[mm]);

  DrawLatex(42, 0.940, 0.945, 0.050, 31, Form("%.3f fb^{-1} (13TeV)", lumi_fb));

  canvas[2]->Modified();
  canvas[2]->Update();
  canvas[2]->SaveAs(outputdir + "/dy_scale.png");
}


//------------------------------------------------------------------------------
// GetScale
//------------------------------------------------------------------------------
void GetScale(int    ch,
	      float& scale_value,
	      float& scale_error,
	      float& R_data_value,
	      float& R_data_error,
	      float& R_dy_value,
	      float& R_dy_error)
{
  TH1D* h_m2l_em = (TH1D*)h2_data[em]->ProjectionY("h_m2l_em", bin_metmin, bin_metmax);
  TH1D* h_m2l_ll = (TH1D*)h2_data[ch]->ProjectionY("h_m2l_ll", bin_metmin, bin_metmax);
  TH1D* h_m2l_dy = (TH1D*)h2_dy  [ch]->ProjectionY("h_m2l_dy", bin_metmin, bin_metmax);
  TH1D* h_m2l_wz = (TH1D*)h2_wz  [ch]->ProjectionY("h_m2l_wz", bin_metmin, bin_metmax);
  TH1D* h_m2l_zz = (TH1D*)h2_zz  [ch]->ProjectionY("h_m2l_zz", bin_metmin, bin_metmax);


  // Initialize counters and errors
  //----------------------------------------------------------------------------
  float n_in_em = 0, err_in_em = 0;
  float n_in_ll = 0, err_in_ll = 0;
  float n_in_dy = 0, err_in_dy = 0;
  float n_in_wz = 0, err_in_wz = 0;
  float n_in_zz = 0, err_in_zz = 0;

  float n_out_em = 0, err_out_em = 0;
  float n_out_ll = 0, err_out_ll = 0;
  float n_out_dy = 0, err_out_dy = 0;
  float n_out_wz = 0, err_out_wz = 0;
  float n_out_zz = 0, err_out_zz = 0;


  // Sum entries and errors^2
  //----------------------------------------------------------------------------
  for (int i=0; i<=h_m2l_em->GetNbinsX()+1; i++)
    {
      // Inside the Z-peak
      if (i >= bin_zmin && i <= bin_zmax)
	{
	  n_in_em += h_m2l_em->GetBinContent(i);
	  n_in_ll += h_m2l_ll->GetBinContent(i);
	  n_in_dy += h_m2l_dy->GetBinContent(i);
	  n_in_wz += h_m2l_wz->GetBinContent(i);
	  n_in_zz += h_m2l_zz->GetBinContent(i);

	  err_in_em += h_m2l_em->GetSumw2()->At(i);
	  err_in_ll += h_m2l_ll->GetSumw2()->At(i);
	  err_in_dy += h_m2l_dy->GetSumw2()->At(i);
	  err_in_wz += h_m2l_wz->GetSumw2()->At(i);
	  err_in_zz += h_m2l_zz->GetSumw2()->At(i);
	}
      // Outside the Z-peak
      else
	{
	  n_out_em += h_m2l_em->GetBinContent(i);
	  n_out_ll += h_m2l_ll->GetBinContent(i);
	  n_out_dy += h_m2l_dy->GetBinContent(i);
	  n_out_wz += h_m2l_wz->GetBinContent(i);
	  n_out_zz += h_m2l_zz->GetBinContent(i);

	  err_out_em += h_m2l_em->GetSumw2()->At(i);
	  err_out_ll += h_m2l_ll->GetSumw2()->At(i);
	  err_out_dy += h_m2l_dy->GetSumw2()->At(i);
	  err_out_wz += h_m2l_wz->GetSumw2()->At(i);
	  err_out_zz += h_m2l_zz->GetSumw2()->At(i);
	}
    }


  // Don't forget to sqrt the errors
  //----------------------------------------------------------------------------
  err_in_em = sqrt(err_in_em);
  err_in_ll = sqrt(err_in_ll);
  err_in_dy = sqrt(err_in_dy);
  err_in_wz = sqrt(err_in_wz);
  err_in_zz = sqrt(err_in_zz);

  err_out_em = sqrt(err_out_em);
  err_out_ll = sqrt(err_out_ll);
  err_out_dy = sqrt(err_out_dy);
  err_out_wz = sqrt(err_out_wz);
  err_out_zz = sqrt(err_out_zz);


  // Compute the estimated number of DY events
  //----------------------------------------------------------------------------
  float n_in_est = n_in_ll - k_value[ch] * n_in_em;

  float err_in_est = sqrt(err_in_ll*err_in_ll + errAB2(k_value[ch], k_error[ch], n_in_em, err_in_em));

  float n_out_est = n_out_ll - k_value[ch] * n_out_em;

  float err_out_est = sqrt(err_out_ll*err_out_ll + errAB2(k_value[ch], k_error[ch], n_out_em, err_out_em));


  // Include the peaking backgrounds
  //----------------------------------------------------------------------------
  if (includeVZ)
     {
      n_in_est -= (n_in_wz + n_in_zz);
  
      err_in_est = sqrt(err_in_est*err_in_est + err_in_wz*err_in_wz + err_in_zz*err_in_zz);
  
      n_out_est -= (n_out_wz + n_out_zz);

      err_out_est = sqrt(err_out_est*err_out_est + err_out_wz*err_out_wz + err_out_zz*err_out_zz);
      }


  // Compute R and the scale factor
  //----------------------------------------------------------------------------
  R_data_value = n_out_est / n_in_est;
  R_data_error = errRatio(n_out_est, err_out_est, n_in_est, err_in_est);

  R_dy_value = n_out_dy / n_in_dy;
  R_dy_error = errRatio(n_out_dy, err_out_dy, n_in_dy, err_in_dy);

  scale_value = n_in_est / n_in_dy;
  scale_error = errRatio(n_in_est, err_in_est, n_in_dy, err_in_dy);
 
 if (R_study)
  {
   R_zz_value[cutt][ch] = n_out_zz / n_in_zz ; 
   R_zz_error[cutt][ch] = errRatio(n_out_zz, err_out_zz, n_in_zz, err_in_zz);	

   R_wz_value [cutt][ch] = n_out_wz / n_in_wz ; 
   R_wz_error [cutt][ch] = errRatio(n_out_wz, err_out_wz, n_in_wz, err_in_wz);	

  if (show_R_study_Results)
  {
  printf("\n");
  printf(" R(DY) %s       %6.3f +- %-5.3f\n", schannel[ch].Data(), R_dy_value,           R_dy_error);
  printf(" R(ZZ) %s       %6.3f +- %-5.3f\n", schannel[ch].Data(), R_zz_value[cutt][ch], R_zz_error[cutt][ch]);
  printf(" R(WZ) %s       %6.3f +- %-5.3f\n", schannel[ch].Data(), R_wz_value[cutt][ch], R_wz_error[cutt][ch]);
 
  }
	
  }

  // Print the results
  //----------------------------------------------------------------------------
  if (printResults)
    {
      printf("\n");
      printf(" k(%s)       %6.3f +- %-5.3f\n", schannel[ch].Data(), k_value[ch],  k_error[ch]);
      printf(" Nin(%s)     %6.0f +- %-5.0f\n", schannel[ch].Data(), n_in_ll,      err_in_ll);
      printf(" NoutData(%s)    %6.0f +- %-5.0f\n", schannel[ch].Data(), n_out_ll,     err_out_ll);
      printf(" Nin(em)     %6.0f +- %-5.0f\n",                      n_in_em,      err_in_em);
      printf(" Nin(WZ)     %6.2f +- %-5.2f\n",                      n_in_wz,      err_in_wz);
      printf(" Nin(ZZ)     %6.2f +- %-5.2f\n",                      n_in_zz,      err_in_zz);
      printf(" Nin(DY)     %6.1f +- %-5.1f\n",                      n_in_dy,      err_in_dy);
      printf(" Nin(est)    %6.1f +- %-5.1f\n",                      n_in_est,     err_in_est);
     printf(" Nout(est)    %6.1f +- %-5.1f\n",                     n_in_est*R_dy_value,  -999.  );
     printf(" Nout(DY)    %6.1f +- %-5.1f\n",                       n_out_dy, err_out_dy   );
//      printf(" Nout_up(est)    %6.1f +- %-5.1f\n",                  n_in_est*0.151,    -999.);
//      printf(" Nout_down(est)    %6.1f +- %-5.1f\n",                n_in_est*0.122,    -999.);
//err_out_est);
      printf("----------------------------\n");

//       printf(" Nout(est)-Nout_up(est)    %6.1f +- %-5.1f\n", n_in_est*0.179 - n_in_est*0.151, -999.); 
//       printf(" Nout(est)-Nout_down(est)    %6.1f +- %-5.1f\n", n_in_est*0.179 - n_in_est*0.122, -999.); 

      printf("----------------------------\n");
      printf(" R(est)      %6.3f +- %-5.3f\n",                      R_data_value, R_data_error);
      printf(" R(DY)       %6.3f +- %-5.3f\n",                      R_dy_value,   R_dy_error);
      printf(" SF(est/DY)  %6.3f +- %-5.3f\n",                      scale_value,  scale_error);
      printf("\n");
    }
}


//------------------------------------------------------------------------------
// errAB2 = err(a*b)^2
//------------------------------------------------------------------------------
float errAB2(float a, float err_a, float b, float err_b)
{
  float err = b*b*err_a*err_a + a*a*err_b*err_b;

  return err;
}


//------------------------------------------------------------------------------
// errRatio = err(a/b)
//------------------------------------------------------------------------------
float errRatio(float a, float err_a, float b, float err_b)
{
  float ratio =fabs(a/b);

  float err = ratio * sqrt((err_a*err_a)/(a*a) + (err_b*err_b)/(b*b));

  return err;
}


//------------------------------------------------------------------------------
// SetGraph
//------------------------------------------------------------------------------
void SetGraph(TGraphErrors* g,
	      Color_t       color,
	      Style_t       style)
{
  g->SetLineColor  (color);
//  g->SetLineWidth  (2);
  g->SetMarkerColor(color);
  g->SetMarkerSize (0.8);
  g->SetMarkerStyle(style);
}


//------------------------------------------------------------------------------
// DrawLegend
//------------------------------------------------------------------------------
TLegend* DrawLegend(Float_t  x1,
		    Float_t  y1,
		    TObject* hist,
		    TString  label,
		    TString  option,
		    Float_t  tsize,
		    Float_t  xoffset,
		    Float_t  yoffset)
{
  TLegend* legend = new TLegend(x1,
				y1,
				x1 + xoffset,
				y1 + yoffset);
  
  legend->SetBorderSize(    0);
  legend->SetFillColor (    0);
  legend->SetTextAlign (   12);
  legend->SetTextFont  (   42);
  legend->SetTextSize  (tsize);

  legend->AddEntry(hist, label.Data(), option.Data());
  legend->Draw();

  return legend;
}


//------------------------------------------------------------------------------
// DrawLatex 
//------------------------------------------------------------------------------
void DrawLatex(Font_t      tfont,
	       Float_t     x,
	       Float_t     y,
	       Float_t     tsize,
	       Short_t     align,
	       const char* text,
	       Bool_t      setndc)
{
  TLatex* tl = new TLatex(x, y, text);

  tl->SetNDC      (setndc);
  tl->SetTextAlign( align);
  tl->SetTextFont ( tfont);
  tl->SetTextSize ( tsize);

  tl->Draw("same");
}
