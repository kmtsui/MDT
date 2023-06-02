void CloneTree(std::string wcsimfile, std::string mdtfile)
{
    TFile oldfile(wcsimfile.c_str());
    
    TFile newfile(mdtfile.c_str(),"update");
    for (auto treename : {"wcsimGeoT", "wcsimRootOptionsT", "fRooTrackerOutputTree"})
    {
        TTree* t = (TTree*)oldfile.Get(treename);
        TTree* nt = t->CloneTree();
        newfile.cd();
        nt->Write();
    }
    newfile.Close();
    oldfile.Close();
}