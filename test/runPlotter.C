#include "../Constants.h"
#include "HistogramReader.h"


const TString inputdir  = "../rootfiles/50ns/";
const TString outputdir = "figures/50ns/";


void runPlotter(TString level)
{
  enum {linY, logY};

  gInterpreter->ExecuteMacro("PaperStyle.C");

  HistogramReader plotter(inputdir, outputdir);

  plotter.SetLuminosity(lumi50ns_fb);

  plotter.AddProcess("01_Data",      " data",    kBlack);
  plotter.AddProcess("08_WJets",     " W+jets",  kAzure-9);
  plotter.AddProcess("06_WW",        " WW",      kAzure-7);
  plotter.AddProcess("05_SingleTop", " top",     kYellow-6);
  plotter.AddProcess("04_Top",       " tt+jets", kYellow);
  plotter.AddProcess("03_ZZ",        " ZZ",      kRed+3);
  plotter.AddProcess("07_ZJets",     " Z+jets",  kGreen+2);
  plotter.AddProcess("02_WZ",        " WZ",      kOrange-2);


  // Draw
  //----------------------------------------------------------------------------
  TString analysis = (level.Contains("WZ")) ? "WZ" : "WW";

  int firstchannel = (analysis.EqualTo("WZ")) ? eee : ee;
  int lastchannel  = (analysis.EqualTo("WZ")) ? lll : ll;

  for (int j=0; j<=njetbin; j++)
    {
      if (analysis.EqualTo("WZ") && j != njetbin) continue;
      
      TString jetbin = (j < njetbin) ? Form("/%djet", j) : "";

      gSystem->mkdir(outputdir + level + jetbin, kTRUE);

      TString prefix = level + jetbin + "/h_";

      for (int i=firstchannel; i<=lastchannel; i++)
	{
	  TString suffix = "_" + schannel[i];


	  // Common histograms
	  //--------------------------------------------------------------------
	  plotter.Draw(prefix + "m2l"        + suffix, "m_{#font[12]{ll}}",                        5, 0, "GeV",  logY, true, 60, 120);
	  plotter.Draw(prefix + "m2l"        + suffix, "m_{#font[12]{ll}}",                        5, 0, "GeV",  linY, true, 60, 120);
	  plotter.Draw(prefix + "counterLum" + suffix, "yield",                                   -1, 0, "NULL", linY);
	  plotter.Draw(prefix + "pfType1Met" + suffix, "E_{T}^{miss}",                             5, 0, "GeV",  linY);
	  plotter.Draw(prefix + "ht"         + suffix, "H_{T}",                                    5, 0, "GeV",  linY);
	  plotter.Draw(prefix + "nvtx"       + suffix, "number of vertices",                      -1, 0, "NULL", linY);
	  plotter.Draw(prefix + "njet"       + suffix, "number of jets (p_{T}^{jet} > 30 GeV)",   -1, 0, "NULL", logY);
	  plotter.Draw(prefix + "nbjet"      + suffix, "number of b-jets (p_{T}^{jet} > 30 GeV)", -1, 0, "NULL", logY);


	  // 3-lepton histograms
	  //--------------------------------------------------------------------
	  if (analysis.EqualTo("WW")) continue;

	  plotter.Draw(prefix + "m3l" + suffix, "m_{#font[12]{3l}}", 5, 0, "GeV", linY, true, 60, 300);


	  // WZ histograms
	  //--------------------------------------------------------------------
	  if (!level.Contains("HasZ")) continue;

	  plotter.Draw(prefix + "zl1pt"        + suffix, "Z leading lepton p_{T}",              5, 0, "GeV",  linY, true, 0, 150);
	  plotter.Draw(prefix + "zl2pt"        + suffix, "Z trailing lepton p_{T}",             5, 0, "GeV",  linY, true, 0, 150);
	  plotter.Draw(prefix + "wlpt"         + suffix, "W lepton p_{T}",                      5, 0, "GeV",  linY, true, 0, 150);
	  plotter.Draw(prefix + "zl1eta"       + suffix, "Z leading lepton #eta",               5, 1, "NULL", linY);
	  plotter.Draw(prefix + "zl2eta"       + suffix, "Z trailing lepton #eta",              5, 1, "NULL", linY);
	  plotter.Draw(prefix + "wleta"        + suffix, "W lepton #eta",                       5, 1, "NULL", linY);
	  plotter.Draw(prefix + "wlzl1_deltar" + suffix, "#DeltaR(W lepton, Z leading lepton)", 5, 1, "NULL", linY);
	  plotter.Draw(prefix + "wlzl2_deltar" + suffix, "#DeltaR(W lepton, Z leading lepton)", 5, 1, "NULL", linY);
	  plotter.Draw(prefix + "wlzl_deltar"  + suffix, "#DeltaR(W lepton, Z leading lepton)", 5, 1, "NULL", linY);
	}
    }

  gSystem->Exec("for dir in $(find ./ -type d); do cp -n ../index.php $dir/; done");
  gSystem->Exec("rm index.php");
}


//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
# ifndef __CINT__
int main(int argc, char ** argv)
{
  if(argc < 2) {
    
    printf("\n rm -rf %s\n\n", outputdir.Data());

    for (int i=0; i<ncut; i++) printf(" ./runPlotter %s\n", scut[i].Data());

    printf("\n");

    exit(0);
  }

  runPlotter(argv[1]);

  return 0;
}
# endif