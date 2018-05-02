void analyze2() {

  TFile *f = new TFile("sam_austen_muonData2.root");
  TTree *MuLife = (TTree *) f->Get("tree");


  
  //create histrograms
  TH1F *up = new TH1F("up","Up and Down Decays with Difference",80,10,4000);
  TH1F *down = new TH1F("down","down",80,10,4000);
  TH1F *diff = new TH1F("diff","Difference of Up and Down Decays",80,10,4000);

  //get total number of events
  Long64_t numEvents = MuLife->GetEntries();

  //create arrays to store event data
  short ADC_1[10];
  short ADC_2[10];
  short STDC_8[10];

  for(int i = 0; i < 10; i++) {
      MuLife->SetBranchAddress(Form("ADC_1_%d",i), &ADC_1[i]);
    }

  for(int i = 0; i < 10; i++) {
      MuLife->SetBranchAddress(Form("ADC_2_%d",i), &ADC_2[i]);
    }

  for(int i = 0; i < 10; i++) {
      MuLife->SetBranchAddress(Form("STDC_8_%d",i), &STDC_8[i]);
    }

  //create variables to track the muon
  int cutOffADC_1[10] = {60,125,200,180,40,310,80,60,60,270};
  int cutOffADC_2[10] = {80,210,60,60,80,60,40,80,60,165};
  int downCount = 0;

  //loop through all events
  for (int i=0;i<numEvents;i++) {
      //get the current event
      MuLife->GetEntry(i);
      //determine what TDC the muon decayed to if there was one, if not skip to next event
      int finalSTDC = -1;
      for(int j = 0; j < 10; j++) {
          //4095 signal was bad
	        if((STDC_8[j] != 4095) && (STDC_8[j] > 0)) {
	            if(finalSTDC == -1)
		          finalSTDC = j;
	          }   
	      } 
      int finalADC = -1;
      for(int j = 0; j < 10; j++) {
          //check if the detected signal is within the cutoff parameters
	        if((ADC_1[j] > cutOffADC_1[j]) && (ADC_2[j] > cutOffADC_2[j])) {
	            if(finalADC == -1)
		            finalADC = j;
	          }
	        else
	          continue;
	      }
      if((finalADC != -1) && (finalSTDC != -1))
	      {
	        if(finalSTDC <= finalADC)
	          {
	            up->Fill(STDC_8[finalSTDC]);
	          }
        }
	    else
	      {
	        if(downCount == 0)
		        {
		          down->Fill(STDC_8[finalSTDC]);
		        }
	            downCount++;
	        if(downCount == 4)
		        downCount = 0;
	      } 
    }


  //Find the difference of up and down events  
  diff->Add(up,1);
  diff->Add(down,-1);

  
  down->SetLineColor(kRed);
  up->SetLineColor(kBlue);
  diff->SetLineColor(kBlack);
  
  up->GetYaxis()->SetRangeUser(-100,450);
  up->GetYaxis()->SetTitle("Counts");
  up->GetYaxis()->CenterTitle();
  up->GetXaxis()->SetTitle("Time(channels)");
  up->GetXaxis()->CenterTitle();

  
  /*diff->GetYaxis()->SetRangeUser(-60,60);
  diff->GetYaxis()->SetTitle("Up-Down Counts");
  diff->GetYaxis()->CenterTitle();
  diff->GetXaxis()->SetTitle("Time(channels)");
  diff->GetXaxis()->CenterTitle();*/
  
  //up->Draw();
  //down->Draw("SAME");
  //diff->Draw("SAME");

  //fitUp();
  //fitDown();
  //fitDiff();
  //diff->Print("all"); > histogram.txt;

  //Save histogram as text file
  ofstream myFile("histogram.txt");
  myFile << diff->Print("all") << endl;
  myFile.close();

}

void fitUp() {
TF1 *ExpFit = new TF1("ExpFit", "([0]*TMath::Exp(-x/[1])+[2])",10,4000);
  ExpFit->SetLineColor(kBlue);
  ExpFit->SetParameter(0,40);
  ExpFit->SetParameter(1,1000);
  ExpFit->SetParameter(2,10);
  //ExpFit->SetParameter(3,2);
  //ExpFit->SetParameter(4,100);
  up->Fit("ExpFit","Rr");
}


void fitDown() {
TF1 *ExpFit = new TF1("ExpFit", "([0]*TMath::Exp(-x/[1])+[2])",10,4000);
  ExpFit->SetParameter(0,40);
  ExpFit->SetParameter(1,1000);
  ExpFit->SetParameter(2,10);
  //ExpFit->SetParameter(3,2);
  //ExpFit->SetParameter(4,100);
  down->Fit("ExpFit","Rr");
}



void fitDiff() {
TF1 *diffFit = new TF1("diffFit", "[0]*TMath::Exp(-x/1100)*TMath::Cos([1]*x+[2])+[3]",10,4000);
  diffFit->SetParameter(0,40);
  diffFit->SetParameter(1,.0015);
  diffFit->SetParameter(2,0);
  diffFit->SetParameter(3,100);
  diff->Fit("diffFit","Rr");
}
