#include "HaplotypeSet.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>


void printErr(String filename)
{
    cout<<" Error in M3VCF File !!! "<<endl;
    cout<<" Please re-construct the following [.m3vcf] file using Minimac2 and try again ..."<<endl;
    cout<< " [ "<< filename<<" ] "<<endl;
    cout<<" Contact author if problem still persists : sayantan@umich.edu "<<endl;
    cout<<" Program Exiting ..."<<endl<<endl;
    abort();
}



void HaplotypeSet::PrintDosageForVcfOutputForIDMaleSamples(IFILE vcfdose, int MarkerIndex,bool majorIsReference,char refAllele)
{

    bool colonIndex;
    for(int hapId=0;hapId<(int)Dosage.size();hapId++)
        {
            char a1=ImputedAlleles[hapId][MarkerIndex];
            colonIndex=false;
            ifprintf(vcfdose,"\t");
            if(GT)
            {
                int outAllele1;
                if(a1==refAllele)
                    outAllele1=0;
                else
                    outAllele1=1;
                ifprintf(vcfdose,"%d",outAllele1);
                colonIndex=true;

            }


            double x=Dosage[hapId][MarkerIndex];

            if(DS)
            {

                if(colonIndex)
                    ifprintf(vcfdose,":");
                colonIndex=false;
                if(majorIsReference)
                    ifprintf(vcfdose,"%.3f",1-x);
                else
                    ifprintf(vcfdose,"%.3f",x);
                colonIndex=true;
            }


            if(GP)
            {

                if(colonIndex)
                    ifprintf(vcfdose,":");
                colonIndex=false;
                double p1,p3;
                if(majorIsReference)
                {
                    p1=x;
                    p3=(1-x);
                }
                else
                {
                    p3=x;
                    p1=(1-x);
                }

                ifprintf(vcfdose,"%.3f,%.3f",p1,p3);

            }


        }


}


void HaplotypeSet::PrintDosageForVcfOutputForID(IFILE vcfdose, int MarkerIndex,bool majorIsReference,char refAllele)
{

    bool colonIndex;
    for(int hapId=0;hapId<(int)Dosage.size()/2;hapId++)
        {
            char a1=ImputedAlleles[2*hapId][MarkerIndex];
            char a2=ImputedAlleles[2*hapId+1][MarkerIndex];
            colonIndex=false;
            ifprintf(vcfdose,"\t");
            if(GT)
            {
                int outAllele1,outAllele2;
                if(a1==refAllele)
                    outAllele1=0;
                else
                    outAllele1=1;
                if(a2==refAllele)
                    outAllele2=0;
                 else
                    outAllele2=1;
                ifprintf(vcfdose,"%d|%d",outAllele1,outAllele2);
                colonIndex=true;

            }


            double x=Dosage[2*hapId][MarkerIndex];
            double y=Dosage[2*hapId+1][MarkerIndex];

            if(DS)
            {

                if(colonIndex)
                    ifprintf(vcfdose,":");
                colonIndex=false;
                if(majorIsReference)
                    ifprintf(vcfdose,"%.3f",1-x+1-y);
                else
                    ifprintf(vcfdose,"%.3f",x+ y);
                colonIndex=true;
            }


            if(GP)
            {

                if(colonIndex)
                    ifprintf(vcfdose,":");
                colonIndex=false;
                double p1,p2,p3;
                if(majorIsReference)
                {
                    p1=x*y;
                    p2=x*(1-y)+y*(1-x);
                    p3=(1-x)*(1-y);
                }
                else
                {
                    p3=x*y;
                    p2=x*(1-y)+y*(1-x);
                    p1=(1-x)*(1-y);
                }

                ifprintf(vcfdose,"%.3f,%.3f,%.3f",p1,p2,p3);

            }


        }


}



void HaplotypeSet::InitializePartialDosageForVcfOutputMaleSamples(int NHaps,
                                                       int NMarkers,vector<bool> &Format)
{
    numHaplotypes = NHaps;
    numMarkers = NMarkers;
    Dosage.resize(numHaplotypes);

    ImputedAlleles.resize(numHaplotypes);
    individualName.resize(numHaplotypes);
    for(int i=0;i<numHaplotypes;i++)
        {
            Dosage[i].resize(NMarkers,3.0);
            ImputedAlleles[i].resize(NMarkers,0.0);

        }

    GT=Format[0];
    DS=Format[1];
    GP=Format[2];



}

void HaplotypeSet::InitializePartialDosageForVcfOutput(int NHaps,
                                                       int NMarkers,vector<bool> &Format)
{
    numHaplotypes = NHaps;
    numMarkers = NMarkers;
    Dosage.resize(numHaplotypes);
    ImputedAlleles.resize(numHaplotypes);
    individualName.resize(numHaplotypes/2);
    for(int i=0;i<numHaplotypes;i++)
        {
            Dosage[i].resize(NMarkers,3.0);
            ImputedAlleles[i].resize(NMarkers,0.0);

        }

    GT=Format[0];
    DS=Format[1];
    GP=Format[2];



}


void HaplotypeSet::SaveDosageForVcfOutputSampleWise(int SamID,string &SampleName, vector<double> &dose1,vector<double> &dose2,
                                                    vector<char> &impAlleles1,vector<char> &impAlleles2)
{
    individualName[SamID]=SampleName;
    Dosage[2*SamID]=dose1;
    ImputedAlleles[2*SamID]=impAlleles1;
    Dosage[2*SamID+1]=dose2;
    ImputedAlleles[2*SamID+1]=impAlleles2;
}

void HaplotypeSet::SaveDosageForVcfOutputSampleWiseChrX(int SamID,string &SampleName, vector<double> &dose1,
                                                    vector<char> &impAlleles1)
{
    individualName[SamID]=SampleName;
    Dosage[SamID]=dose1;
    ImputedAlleles[SamID]=impAlleles1;
}


void HaplotypeSet::SaveDosageForVcfOutput(int hapID,vector<double> dose,vector<char> impAlleles)
{

    Dosage[hapID]=dose;
    ImputedAlleles[hapID]=impAlleles;

}




void HaplotypeSet::reconstructHaplotype(vector<char> &reHaplotypes,int &index)
{
    if((int)reHaplotypes.size()!=numMarkers)
        {
            cout<<" SIZE MISMATCH "<<endl;
            abort();
        }

    int markerIndex=0,k;
    char checkAllele=0;

    for(int j=0;j<(int)ReducedStructureInfo.size();j++)
    {

        for(k=0;k<(ReducedStructureInfo[j].endIndex-ReducedStructureInfo[j].startIndex);k++)
        {

            reHaplotypes[markerIndex++]=ReducedStructureInfo[j].uniqueHaps[ReducedStructureInfo[j].uniqueIndexMap[index]][k];
            if(k==0 && j>1)
            {
                if(checkAllele!=(ReducedStructureInfo[j].uniqueHaps[ReducedStructureInfo[j].uniqueIndexMap[index]][k]))
                {
                    cout<<index<<"\t"<<j<<"\t"<<k<<"\t"<<(int)checkAllele<<"\t"<<(int)(ReducedStructureInfo[j].uniqueHaps[ReducedStructureInfo[j].uniqueIndexMap[index]][k])<<endl;
                    cout<<" CHECK ALLELEE  MISMATCH "<<endl;
                    abort();
                }

            }

        }

        if(j==((int)ReducedStructureInfo.size()-1))
        {

            reHaplotypes[markerIndex]=ReducedStructureInfo[j].uniqueHaps[ReducedStructureInfo[j].uniqueIndexMap[index]][k];
        }
        else
        {
            checkAllele=ReducedStructureInfo[j].uniqueHaps[ReducedStructureInfo[j].uniqueIndexMap[index]][k];
        }

    }
}


bool HaplotypeSet::readm3vcfFile(String m3vcfFile,String CHR,int START,int END,int WINDOW)
{

    typedef boost::tokenizer< boost::char_separator<char> > wsTokenizer;
    wsTokenizer::iterator i,j,k,l;
	boost::char_separator<char> Tabsep("\t"),EqSep("="),semSep(";"),comSep(","),barSep("|"),dashSep("-");
    string line,tempString,tempBlockPos,tempString2,tempString3,tempName,tempChr;
    variant tempVariant;
    variant tempVariant2;
    int InitialNMarkers=0,blockIndex,startIndexFlag=0,readIndex=0,writeBlockIndex=0,NoBlocks,tempPos,NoMarkersWritten=0,tempVarCount,tempRepCount;

    cout<<"\n Reading Reference Haplotype information from M3VCF files : "<<m3vcfFile<<endl<<endl;
    int OrigStartPos=START;
    int OrigEndPos=END;
    if (WINDOW > 0)
    {
        if (START-WINDOW < 0)
            START = 0;
        else
            START -= WINDOW;

        END += WINDOW;
    }
    if(CHR!="")
    {
        std::cout << "\n Loading markers from chromosome " << CHR;
        if(END>0)
            std::cout << " from base position "<<START<<" to base position "<<END<<"."<< endl;
        else
            std::cout << " from base position "<<START<<" till end of M3VCF file."<< endl;
    }


    PrintStartIndex=0;
    PrintEndIndex=0;
    IFILE m3vcfxStream = ifopen(m3vcfFile, "r");

    if(m3vcfxStream)
    {

        {
        m3vcfxStream->readLine(line);
        if(line.compare("##fileformat=M3VCF")!=0 && line.compare("##fileformat=OPTM")!=0)
        {
            cout<<" Incorrect Header Information : "<<line<<endl;
            cout<<" Header line should be : ##fileformat=M3VCF "<<endl<<endl;
            printErr(m3vcfFile);
        }

        bool Header=true;

        while(Header)
        {
            line.clear();
            m3vcfxStream->readLine(line);
            if(line.substr(0,2).compare("##")==0)
                Header=true;
            else
                break;

            wsTokenizer t(line,EqSep);
            i = t.begin();
            string tempString=i->c_str();

            if(tempString.compare("##n_blocks")==0)
            {
                ++i;
                NoBlocks=boost::lexical_cast<int>(i->c_str());
                continue;

            }
            else if(tempString.compare("##n_haps")==0)
            {
                ++i;
                numHaplotypes=boost::lexical_cast<int>(i->c_str());
                continue;

            }
            else if(tempString.compare("##n_markers")==0)
            {
                ++i;
                InitialNMarkers=boost::lexical_cast<int>(i->c_str());
                continue;

            }
            else if(tempString.compare("##chrxRegion")==0)
            {
                ++i;
                tempString=i->c_str();
                if(tempString.compare("NonPseudoAutosomal")==0)
                    PseudoAutosomal=false;
                else if(tempString.compare("PseudoAutosomal")==0)
                    PseudoAutosomal=true;
                else
                {
                    cout << "\n Inconsistent Tag for Chr X. "<<endl;
                    cout << " Please check the file properly..\n";
                    cout << " Program Aborting ... "<<endl;
                    return false;

                }
                continue;

            }

        }

        int colCount=0;
        wsTokenizer t4(line,Tabsep);
        i = t4.begin();
        while(i!=t4.end())
        {
            colCount++;
            if(colCount>9)
            {
                tempString2=i->c_str();
//                if(colCount%2==0)
                individualName.push_back(tempString2.substr(0,tempString2.size()-6));
            }
            ++i;
        }

        cout<<" Reading  "<<numHaplotypes<< " haplotypes from data ..."<<endl<<endl;
        if((int)individualName.size()!=numHaplotypes)
        {
            cout<<endl<<" Error in Data consistency !!! "<<endl<<endl;
            cout<<" Number of Haplotypes should be : "<< numHaplotypes<<", but "<<individualName.size()<<" Haplotypes found in header row !!!"<< endl<<endl;
            printErr(m3vcfFile);
        }


        if(individualName.size()==0)
        {
            cout << "\n No haplotypes recorded from M3VCF Input File : "<<m3vcfFile<<endl;
            cout << " Please check the file properly..\n";
            cout << " Program Aborting ... "<<endl;
            return false;
        }

        }

        for(blockIndex=0;blockIndex<NoBlocks;blockIndex++)
        {


            if (blockIndex % 1000 == 0)
			{
			    printf("  Loading Block %d out of %d blocks to be loaded... [%.1f%%] "
                , blockIndex + 1, NoBlocks, 100*(double)(blockIndex + 1)/(double)NoBlocks);
                cout<<endl;
            }

            int flag=0,blockEnterFlag=0,blocktoSave=0;
            line.clear();
            m3vcfxStream->readLine(line);
            wsTokenizer t5(line,Tabsep);
            i = t5.begin();
            tempChr=i->c_str();
            ++i;
            tempBlockPos=i->c_str();
            wsTokenizer t10(tempBlockPos,dashSep);
            k=t10.begin();
            int tempStartBlock=boost::lexical_cast<int>(k->c_str());
            ++k;
            int tempEndBlock=boost::lexical_cast<int>(k->c_str());

            if(tempStartBlock)


            if(CHR!="")
            {
                if(tempChr.compare(CHR.c_str())!=0)
                    flag=1;
                else
                {
                    if(END>0)
                    {
                        if(tempStartBlock>END)
                            flag=1;
                    }
                    if(tempEndBlock<START)
                        flag=1;
                }
            }





            ++i;
            string blockName=i->c_str();

            ++i;
            ++i;
            ++i;
            ++i;
            ++i;
            tempString2=i->c_str();


            wsTokenizer t6(tempString2,semSep);

            j=t6.begin();
            tempString3="B"+ boost::lexical_cast<std::string>(blockIndex+1);
            if(tempString3.compare(j->c_str())!=0)
            {
                cout<<endl<<" Error in INFO column (Block Identifier) for block : "<<blockName <<endl;
                cout<<" Block Identifier should be : "<< tempString3<<" but is : "<<j->c_str()<<endl<<endl;
                printErr(m3vcfFile);
            }


            tempString3=(++j)->c_str();
            wsTokenizer t7(tempString3,EqSep);
            k=t7.begin();
            ++k;
            tempVarCount=boost::lexical_cast<int>(k->c_str());
            tempString3=(++j)->c_str();
            wsTokenizer t8(tempString3,EqSep);
            k=t8.begin();
            ++k;
            tempRepCount=boost::lexical_cast<int>(k->c_str());
            ReducedHaplotypeInfo tempBlock;

            if(flag==1)
            {
                for(int tempIndex=0;tempIndex<tempVarCount;tempIndex++)
                m3vcfxStream->discardLine();
                    continue;
            }


            tempBlock.uniqueCardinality.resize(tempRepCount,0.0);
            tempBlock.uniqueHaps.resize(tempRepCount);
            tempBlock.uniqueIndexMap.resize(numHaplotypes);


            ++i;
            ++i;
            int check=0;
            while(i!=t5.end())
            {
                int tempval=boost::lexical_cast<int>(i->c_str());
                tempBlock.uniqueIndexMap[check]=tempval;
                tempBlock.uniqueCardinality[tempval]++;

               check++;
                ++i;
            }


            if(check!=numHaplotypes)
            {
                cout<<endl<<" Error in Data consistency !!! "<<endl;
                cout<<" Number of Haplotypes should be : "<< numHaplotypes<<", but "<<check<<" indices found in block"<<blockName << endl<<endl;
                printErr(m3vcfFile);
            }




            for(int tempIndex=0;tempIndex<tempVarCount;tempIndex++)
            {
                //cout<<numMarkers<<VariantList.size()<<refAlleleList.size()<<markerName.size()<<endl;
                flag=0;
                line.clear();
                m3vcfxStream->readLine(line);

//                 cout<<line<<endl;
//                 line.clear();
//
//               optmxStream->readTilTab(line);
//
//                cout<<line<<endl;

//                if(flag==1)
//                    continue;
                wsTokenizer t9(line,Tabsep);
                l=t9.begin();
                tempChr=l->c_str();
                tempPos=boost::lexical_cast<int>((++l)->c_str());
                tempName=((++l)->c_str());

                 if(CHR!="")
                {
                    if(tempChr.compare(CHR.c_str())!=0)
                        flag=1;
                    else
                    {
                        if(END>0)
                        {
                            if(tempPos>END || tempPos<START)
                                flag=1;
                        }
                        else
                            if(tempPos<START)
                                flag=1;
                         if(tempIndex==(tempVarCount-1) && writeBlockIndex==0)
                            flag=1;
                    }

                }
                readIndex++;

                if(flag==1)
                    continue;
                else
                {
                    if(blockEnterFlag==0)
                        tempBlock.startIndex=writeBlockIndex;
                    else
                        tempBlock.endIndex=writeBlockIndex;
                    if(tempIndex<(tempVarCount-1) && tempIndex>0) // to ensure a single marker from a block is NOT read.
                        blocktoSave=1;
                    blockEnterFlag=1;

                }

                if(tempPos>=OrigStartPos && startIndexFlag==0)
                    {
                        PrintStartIndex=writeBlockIndex;
                        startIndexFlag=1;
                    }

                if(CHR=="" || tempPos<=OrigEndPos)
                    PrintEndIndex=writeBlockIndex;

                variant tempVariant;

                tempVariant.assignValues(tempName,tempChr,tempPos);
                double tempRecom=-3.0,tempError=0.0;

                string tempString98=((++l)->c_str());
                string tempString99=((++l)->c_str());
                tempVariant.assignRefAlt(tempString98,tempString99);
                //markerName[tempIndex+NoMarkersWritten]=tempName;
                ++l;
                ++l;
                tempString=((++l)->c_str());
                wsTokenizer t10(tempString,semSep);
                k=t10.begin();
                tempString3="B"+ boost::lexical_cast<std::string>(blockIndex+1)+".M"+ boost::lexical_cast<std::string>(tempIndex+1);
                if(tempString3.compare(k->c_str())!=0)
                {
                    cout<<endl<<" Error in INFO column (Block Identifier) for variant : "<<tempName <<endl;
                    cout<<" Block Identifier should be : "<< tempString3<<" but is : "<<k->c_str()<<endl<<endl;
                    printErr(m3vcfFile);
                }

                tempString3=(++k)->c_str();
                wsTokenizer t11(tempString3,EqSep);
                j=t11.begin();
                ++j;
                tempVariant.refAllele=(char)(boost::lexical_cast<int>(j->c_str()));



                tempString3=(++k)->c_str();
                wsTokenizer t19(tempString3,EqSep);
                j=t19.begin();
                ++j;
                tempVariant.altAllele=(char)(boost::lexical_cast<int>(j->c_str()));


                if((++k)!=t10.end())
                {

                    tempString3=(k)->c_str();
                    wsTokenizer t20(tempString3,EqSep);
                    j=t20.begin();
                    ++j;
                    tempError=(boost::lexical_cast<double>(j->c_str()));

                    if((++k)!=t10.end())
                    {
                        tempString3=(k)->c_str();
                        wsTokenizer t24(tempString3,EqSep);
                        j=t24.begin();
                        ++j;
                        tempRecom=(boost::lexical_cast<double>(j->c_str()));
                    }
                }


                if(tempIndex<(tempVarCount-1) || blockIndex==(NoBlocks-1))
                {

                    VariantList.push_back(tempVariant);
                    refAlleleList.push_back(tempVariant.refAllele);
                    markerName.push_back(tempName);
                    if(tempRecom!=-3.0)
                    {
                        Recom.push_back(tempRecom);
                        Error.push_back(tempError);
                    }
                    writeBlockIndex++;
                }

                tempString=((++l)->c_str());
                for(check=0;check<tempRepCount;check++)
                {

                    char t=tempString[check];
                    if(t=='0')
                        tempBlock.uniqueHaps[check].push_back(tempVariant.refAllele);
                    else
                        tempBlock.uniqueHaps[check].push_back(tempVariant.altAllele);

                }

            }

            NoMarkersWritten+=(tempVarCount-1);
            if(blocktoSave==1)
                {
//
//            if((tempBlock.endIndex-tempBlock.startIndex+1)!=tempBlock.uniqueHaps[0].size())
//                abort();
                optEndPoints.push_back(tempBlock.startIndex);

                ReducedStructureInfo.push_back(tempBlock);
                }



        }
        if(ReducedStructureInfo.size()>0)
            optEndPoints.push_back(ReducedStructureInfo[ReducedStructureInfo.size()-1].endIndex);

        if(Recom.size()>0)
            Recom.pop_back();

        finChromosome=tempChr;

    }
    else
    {
        cout<<" Following M3VCF File Not Available : "<<m3vcfFile<<endl;
        cout<<" Program Exiting ... "<<endl<<endl;
        return false;
    }


    numMarkers=writeBlockIndex;

    cout<<endl<<" Reference Haplotype information succesfully recorded. "<<endl;


    if(finChromosome=="X")
    {
        cout<<"\n Chromosome X Detected !!! \n";
    }

	std::cout << "\n Number of Markers in File                           : " << InitialNMarkers << endl;
	std::cout << "\n Number of Markers Recorded                          : " << numMarkers << endl;
    std::cout << " Number of Haplotypes Recorded                       : " << numHaplotypes << endl;

    ifclose(m3vcfxStream);

   if(numMarkers<2)
    {
        cout << "\n None/Single marker left after filtering from Input File : "<<m3vcfFile<<endl;
		cout << " Please check the file or the filtering options properly ...\n";
		cout << " Program Aborting ... "<<endl;
		return false;
    }



    std::cout << "\n Haplotype Set successfully loaded from M3VCF File     : " << m3vcfFile << endl;

    return true;




}


void HaplotypeSet::Create(vector<char> &tempHaplotype)
{

    numHaplotypes=1;
    numMarkers=(int)tempHaplotype.size();
    missing.resize(numMarkers,false);
    ScaffoldIndex.resize(numMarkers);

    haplotypesUnscaffolded.push_back(tempHaplotype);

    //cout<<" DERP = "<<tempHaplotype.size()<<endl;
    for(int i=0;i<numMarkers;i++)
        ScaffoldIndex[i]=i;
}



bool CheckValidChrom(string chr)
{
    bool result=false;
    string temp[]={"1","2","3","4","5","6","7","8","9","10","11"
                    ,"12","13","14","15","16","17","18","19","20","21","22","X"};
    std::vector<string> ValidChromList (temp, temp + sizeof(temp) / sizeof(string) );

    for(int counter=0;counter<(int)ValidChromList.size();counter++)
        if(chr==ValidChromList[counter])
            result=true;

    return result;

}

void HaplotypeSet::writem3vcfFile(String &filename,bool &gzip)
{

    std::cout << "\n Writing reduced haplotype information to [.m3vcf] file  : " <<filename+".m3vcf" + (gzip ? ".gz" : "")<< endl<<endl;


    IFILE m3vcffile = ifopen(filename + ".m3vcf" + (gzip ? ".gz" : ""), "wb",(gzip ? InputFile::BGZF : InputFile::UNCOMPRESSED));
    ifprintf(m3vcffile, "##fileformat=M3VCF\n");
    ifprintf(m3vcffile, "##version=1.2\n");
    ifprintf(m3vcffile, "##compression=block\n");
    ifprintf(m3vcffile, "##n_blocks=%d\n",ReducedStructureInfo.size());
    ifprintf(m3vcffile, "##n_haps=%d\n",numHaplotypes);
    ifprintf(m3vcffile, "##n_markers=%d\n",numMarkers);
    if(finChromosome=="X")
        ifprintf(m3vcffile, "##chrxRegion=%s\n",PseudoAutosomal?"PseudoAutosomal":"NonPseudoAutosomal");
    ifprintf(m3vcffile, "##<Note=This is NOT a VCF File and cannot be read by vcftools>\n");
    ifprintf(m3vcffile, "#CHROM\t");
    ifprintf(m3vcffile, "POS\t");
    ifprintf(m3vcffile, "ID\t");
    ifprintf(m3vcffile, "REF\t");
    ifprintf(m3vcffile, "ALT\t");
    ifprintf(m3vcffile, "QUAL\t");
    ifprintf(m3vcffile, "FILTER\t");
    ifprintf(m3vcffile, "INFO\t");
    ifprintf(m3vcffile, "FORMAT");
    int i,j,k;

    for(i=0;i<(int)individualName.size();i++)
    {
        ifprintf(m3vcffile, "\t%s_HAP_1",individualName[i].c_str());

        if(finChromosome!="X")
            ifprintf(m3vcffile, "\t%s_HAP_2",individualName[i].c_str());
        else if(SampleNoHaplotypes[i]==2)
            ifprintf(m3vcffile, "\t%s_HAP_2",individualName[i].c_str());
    }
    ifprintf(m3vcffile, "\n");

    int length=(int)ReducedStructureInfo.size();
    string cno;

    for(i=0;i<length;i++)
    {

        cno=VariantList[ReducedStructureInfo[i].startIndex].chr;
//        int startBp=VariantList[ReducedStructureInfo[i].startIndex].bp;
//        int endBp=VariantList[ReducedStructureInfo[i].endIndex].bp;
        int nvariants=ReducedStructureInfo[i].endIndex-ReducedStructureInfo[i].startIndex+1;
        int reps=ReducedStructureInfo[i].uniqueCardinality.size();


        ifprintf(m3vcffile, "%s\t",cno.c_str());
        ifprintf(m3vcffile, "%d-%d\t",VariantList[ReducedStructureInfo[i].startIndex].bp,VariantList[ReducedStructureInfo[i].endIndex].bp);
        ifprintf(m3vcffile, "<BLOCK:%d-%d>\t.\t.\t.\t.\t",ReducedStructureInfo[i].startIndex,ReducedStructureInfo[i].endIndex);

        ifprintf(m3vcffile, "B%d;VARIANTS=%d;REPS=%d\t.",i+1,nvariants,reps);


        for(j=0;j<numHaplotypes;j++)
            ifprintf(m3vcffile, "\t%d",ReducedStructureInfo[i].uniqueIndexMap[j]);

        ifprintf(m3vcffile, "\n");

        for(j=0;j<nvariants;j++)
        {
            ifprintf(m3vcffile, "%s\t",cno.c_str());
            ifprintf(m3vcffile, "%d\t",VariantList[j+ReducedStructureInfo[i].startIndex].bp);
            ifprintf(m3vcffile, "%s\t",VariantList[j+ReducedStructureInfo[i].startIndex].name.c_str());
            ifprintf(m3vcffile, "%s\t%s\t.\t.\t",VariantList[j+ReducedStructureInfo[i].startIndex].refAlleleString.c_str(),VariantList[j+ReducedStructureInfo[i].startIndex].altAlleleString.c_str());
            ifprintf(m3vcffile, "B%d.M%d;R=%d;A=%d",i+1,j+1,VariantList[j+ReducedStructureInfo[i].startIndex].refAllele,
                     VariantList[j+ReducedStructureInfo[i].startIndex].altAllele);
            if(Error.size()>0)
                ifprintf(m3vcffile, ";Err=%.5g;Recom=%.5g",
                     Error[j+ReducedStructureInfo[i].startIndex],(j+ReducedStructureInfo[i].startIndex)<(int)Recom.size()?Recom[j+ReducedStructureInfo[i].startIndex]:0);
            ifprintf(m3vcffile, "\t");

            for(k=0;k<reps;k++)
            {
                ifprintf(m3vcffile,"%d",ReducedStructureInfo[i].uniqueHaps[k][j]==VariantList[j+ReducedStructureInfo[i].startIndex].refAllele? 0:1);
            }
            ifprintf(m3vcffile, "\n");
        }
    }

    std::cout << " Successfully written file ... "<<endl;
    ifclose(m3vcffile);

}

string HaplotypeSet::DetectTargetFileType(String filename)
{
    cout<<"\n Detecting Target File Type ... "<<endl;
    typedef boost::tokenizer< boost::char_separator<char> > wsTokenizer;
    wsTokenizer::iterator i;
	boost::char_separator<char> Sep(" \t");
    IFILE fileStream = ifopen(filename, "r");

    string line;
    if(fileStream)
    {
        fileStream->readLine(line);
        if(line.length()<1)
            return "Invalid";
        string temp=line.substr(0,17);
        boost::to_lower(temp);
         if(temp.compare("##fileformat=vcfv")==0)
            {
                return "vcf";
            }
        wsTokenizer t(line,Sep);
        i = t.begin();
        if(++i!=t.end())
        {

            string haplo1="HAPLO1";
            if(haplo1.compare(i->c_str())==0)
            {
                return "mach";
            }
            else
                return "Invalid";
        }
        else
            return "Invalid";
    }
    else
    {
        return "NA";
    }

    ifclose(fileStream);

}

string HaplotypeSet::DetectReferenceFileType(String filename)
{
    IFILE fileStream = ifopen(filename, "r");
    string line;
    if(fileStream)
    {
        fileStream->readLine(line);
        if(line.length()<1)
            return "Invalid";
        string temp=line.substr(0,17);
        boost::to_lower(temp);
        if(temp.compare("##fileformat=m3vc")==0)
        {
            return "m3vcf";
        }
        else if(temp.compare("##fileformat=vcfv")==0)
        {
            return "vcf";
        }
        else
            return "Invalid";

    }
    else
    {
        return "NA";
    }


    ifclose(fileStream);
}

bool HaplotypeSet::FastLoadHaplotypes(String filename, int maxIndiv, int maxMarker,String CHR,
                                      int START,int END,int WINDOW,bool rsid,bool compressOnly,bool filter)
{

    cout<<"\n Detecting Reference File Type ... "<<endl;

    string FileType=DetectReferenceFileType(filename);

    if(FileType.compare("m3vcf")==0)
    {
        cout<<"\n Format = M3VCF (Minimac3 VCF File) "<<endl;
        if(compressOnly)
        {
            cout << "\n Reference File provided by \"--refHaps\" is an M3VCF file !!! \n";
            cout << " M3VCF files cannot be processed further !!! "<<endl;
            return false;
        }


        return readm3vcfFile(filename,CHR,START,END,WINDOW);
    }
    else if(FileType.compare("NA")==0)
    {
        cout<<"\n Following File File Not Available : "<<filename<<endl;
        return false;
    }
    else if(FileType.compare("Invalid")==0)
    {

        cout << "\n Reference File provided by \"--refHaps\" must be a VCF or M3VCF file !!! \n";
        cout << " Please check the following file : "<<filename<<endl;
        return false;
    }
    cout<<"\n Format = VCF (Variant Call Format) "<<endl;
    vcfType=true;

    optEndPoints.clear();
	VcfFileReader inFile;
	VcfHeader header;
	VcfRecord record;

	if (!inFile.open(filename, header))
	{
		cout << "\n Program could NOT open file : " << filename << endl;
		return false;
	}

    std::cout << "\n Loading Reference Haplotype Set from VCF File       : " << filename << endl;
//    if(END==0)
//        END=300000000;
    int OrigStartPos=START;
    int OrigEndPos=END;
    if (WINDOW > 0)
    {
        if (START-WINDOW < 0)
            START = 0;
        else
            START -= WINDOW;

        END += WINDOW;
    }
    if(CHR!="")
    {
        std::cout << "\n Region specified by user (including window = "<<WINDOW <<" bp) : chr" << CHR<<":"<<START <<"-"<< (END > 0 ? boost::lexical_cast<string>(END) :"END") << endl;
    }


    PrintStartIndex=0;
    PrintEndIndex=0;
    int     numReadRecords = 0;
	int numtoBeWrittenRecords = 0;
	vector<bool> importIndex;
    int haplotype_index = 0;
	int numSamplesRead = 0;
	int notBiallelic = 0;
	int failFilter = 0;
	int duplicates = 0;
	int insertions = 0;
	int deletions = 0;
	int inconsistent=0;
	string prevID="";
//	bool isInDel = false;
	inFile.setSiteOnly(true);
	numSamplesRead = header.getNumSamples();

	for (int i = 0; i < numSamplesRead; i++)
	{
		string tempName(header.getSampleName(i));
		individualName.push_back(tempName);
	}

    string refAllele,altAllele,PrefrefAllele,PrevaltAllele,cno,fixCno,id;
    int bp;
	cout << "\n Reading VCF File to calculate number of records ... \n";
    cout<<endl;
	// pre-calculate number of samples read and number of markers read to allocate memory.
	while (inFile.readRecord(record))
	{

        int flag = 0;
        if (maxMarker != 0 && numtoBeWrittenRecords>=maxMarker)
            break;

		++numReadRecords;

		if (record.getNumAlts()>1)
		{
			notBiallelic++;
			flag = 1;
		}
		if (record.getFilter().getString(0).compare("PASS") != 0)
		{
			failFilter++;
			if(filter)
                flag = 1;
		}
//		isInDel=false;

		refAllele = record.getRefStr();
		cno=record.getChromStr();
        bp=record.get1BasedPosition();
        id=record.getIDStr();
        altAllele = record.getAltStr();


        if(numReadRecords==1 && CHR=="")
        {
            if(!CheckValidChrom(cno))
            {
                cout << "\n Error !!! Reference VCF File contains chromosome : "<<cno<<endl;
                cout << " VCF File can only contain chromosomes 1-22 and X !!! "<<endl;
                cout << " Program Aborting ... "<<endl;
                return false;
            }

        }
        if(CHR=="" && fixCno!=cno && numReadRecords>1)
        {
            cout << "\n Error !!! Reference VCF File contains multiple chromosomes : "<<cno<<", "<<fixCno<<", ... "<<endl;
            cout << " Please use VCF file with single chromosome or specify chromosome using \"--chr\" option !!! "<<endl;
            cout << " Program Aborting ... "<<endl;
            return false;
        }


        fixCno=cno;

        string currID;
        if(rsid)
            currID=record.getIDStr();
        else
            currID=boost::lexical_cast<string>(cno)+":"+boost::lexical_cast<string>(bp);

        //cout<<prevID<<endl;
        if (strlen(refAllele.c_str()) == 1 && strlen(altAllele.c_str()) == 1)
        {
            switch (refAllele[0])
            {
                case 'A': case 'a': break;
                case 'C': case 'c': break;
                case 'G': case 'g': break;
                case 'T': case 't': break;
                case 'D': case 'd': break;
                case 'I': case 'i': break;
                case 'R': case 'r': break;
                default:
                {
                   cout << "\n WARNING !!! ";
                   cout << " Reference allele for marker : " << currID << " is : " <<refAllele<<endl;
//                   cout<<" In VCF File : " << filename;
//                   cout << "\n VCF reference alleles for SNPs (not INDELs/SVs) can only be A(a), C(c), G(g), or T(t).";
                   cout << " Variant will be ignored... \n";
                   flag=1;
                   inconsistent++;
                }
            }

            if(flag==0)
                switch (altAllele[0])
                    {
                    case 'A': case 'a': break;
                    case 'C': case 'c': break;
                    case 'G': case 'g': break;
                    case 'T': case 't': break;
                    case 'D': case 'd': break;
                    case 'I': case 'i': break;
                    case 'R': case 'r': break;
                    default:
                    {
                        cout << "\n WARNING !!! ";
                        cout << " Alternate allele for marker : " <<currID << " is : " <<altAllele<<endl;
//                        cout<<" In VCF File : " << filename;
//                        cout << "\n VCF alternate alleles for SNPs (not INDELs/SVs) can only be A(a), C(c), G(g), or T(t).";
                        cout << " Variant will be ignored... \n";
                        flag=1;
                    }
                }
        }
        else if(strlen(refAllele.c_str())<strlen(altAllele.c_str()))
        {
//			isInDel = true;
			insertions++;
		}
		else
        {
//			isInDel = true;
			deletions++;
        }


        if(prevID==currID)
        {
            if(refAllele==PrefrefAllele && altAllele==PrevaltAllele)
            {

                cout << " WARNING !!! Duplicate Variant found chr:"<<boost::lexical_cast<string>(cno)+":"+boost::lexical_cast<string>(bp)<<" with identical REF = "<<refAllele
                 <<" and ALT = "<<altAllele <<"\n";
                duplicates++;
            }

        }


//        prevSNP=1-isInDel;
        prevID=currID;
        PrefrefAllele=refAllele;
        PrevaltAllele=altAllele;

		if(CHR!="")
        {
            if(cno.compare(CHR.c_str())!=0)
                flag=1;
            else
            {
                if(END>0)
                {
                    if(bp>END || bp<START)
                        flag=1;
                }
                else
                    if(bp<START)
                        flag=1;
            }

        }

//cout<<fixCno<<"\t"<<cno<<endl;


		if (flag == 0)
		{
			if(bp<OrigStartPos)
                PrintStartIndex++;

            if(CHR=="" || bp<=OrigEndPos)
                PrintEndIndex=numtoBeWrittenRecords;
			++numtoBeWrittenRecords;
			markerName.push_back(currID);
            variant thisVariant(currID,cno,bp);
            VariantList.push_back(thisVariant);
			importIndex.push_back(true);

		}
		else
		{
			importIndex.push_back(false);
		}

	}


	inFile.close();


    if(CHR=="")
        finChromosome=cno;
    else
        finChromosome=CHR.c_str();




	std::cout << "\n Number of Markers read from VCF File                : " << numReadRecords << endl;
	std::cout << " Number of Markers with more than One Allele         : " << notBiallelic << endl;
	std::cout << " Number of Markers failing FILTER = PASS             : " << failFilter << endl;
    std::cout << " Number of Markers with inconsistent Ref/Alt Allele  : " << inconsistent << endl;
    std::cout << " Number of Markers with duplicate ID/Position        : " << duplicates << endl;
    std::cout << " Number of Insertions                                : " << insertions << endl;
	std::cout << " Number of Deletions                                 : " << deletions << endl;

	if (maxIndiv == 0)
		maxIndiv = numSamplesRead;
	numMarkers = numtoBeWrittenRecords;
	numHaplotypes = (maxIndiv<numSamplesRead) ? (2 * maxIndiv) : (2 * numSamplesRead);
    numSamples=numHaplotypes/2;
    if(finChromosome!="X")
    {
        std::cout << "\n Number of Markers to be Recorded                    : " << numtoBeWrittenRecords << endl;
        std::cout << " Number of Haplotypes to be Recorded                 : " << (numHaplotypes) << endl;
    }
    else
    {
        cout<<"\n Chromosome X Detected !!! \n";
        std::cout << "\n Number of Markers to be Recorded                    : " << numtoBeWrittenRecords << endl;
        PseudoAutosomal=false;
        inFile.open(filename, header);
        inFile.setSiteOnly(false);
        int numWrittenRecords = 0;
//        int readIndex = -1;
        inFile.readRecord(record);
        int tempHapCount=0,MaleCount=0,FemaleCount=0;
        for (int i = 0; i<(numSamples); i++)
        {
            if(record.getNumGTs(i)==0)
            {
                std::cout << "\n Empty Value for Individual : " << individualName[i] << " at Marker : " << VariantList[numWrittenRecords].name << endl;
                std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                return false;
            }
            else
            {
                if(record.getNumGTs(i)==1)
                    MaleCount++;
                else
                    FemaleCount++;
                SampleNoHaplotypes.push_back(record.getNumGTs(i));

                tempHapCount+=record.getNumGTs(i);
            }

        }
        inFile.close();
        std::cout << " Number of Samples (Haplotypes) to be Recorded       : " << numSamples << " ("<<tempHapCount <<") "<<endl;
        numHaplotypes=tempHapCount;

        if(MaleCount>0)
        {
        std::cout << " Number of MALE Samples (Haplotypes)                 : " << MaleCount << " ("<<MaleCount <<") "<<endl;
        std::cout << " Number of FEMALE Samples (Haplotypes)               : " << FemaleCount<< " ("<<FemaleCount*2 <<") "<<endl;
        }
        else
        {
            std::cout << "\n All " << numSamples<<" samples have two alleles on Chromosome X !!! "<< endl;
            cout<<" Cannot determine number of male/female samples from Pseudo-Autosomal Region ..."<<endl;
            PseudoAutosomal=true;
        }


    }


   if(numtoBeWrittenRecords<2)
    {
        cout << "\n None/Single marker left after filtering from Input File : "<<filename<<endl;
		cout << " Please check the file or the filtering options properly ...\n";
		cout << " Program Aborting ... "<<endl;
		return false;
    }

    std::cout <<"\n Starting to load data ...";



    int    blockSize = 500;
    int    bufferSize = 10000;
    int LastflushPos=0;
    vector<int> index(numHaplotypes),oldIndex;
    vector<int> previousDifference(numHaplotypes);
    vector<int> previousPredecessor(numHaplotypes);
    vector<int> firstDifference(numHaplotypes-1,0);
    vector<int> cost(bufferSize+1,0);
    vector<int> bestSlice(bufferSize+1,0);
    vector<int> bestComplexity(bufferSize+1,0);
    vector<vector<int> > bestIndex(bufferSize+1);

    findUnique RefUnique;
    vector<String> Haplotypes(numHaplotypes);
    double blockedCost = 0.0;
    for(int i=0;i<numHaplotypes;i++)
        index[i]=i;
    individualName.resize(numSamples);
    // open file again to start loading the data in the variable haplotype.

    cout<<endl;
	inFile.open(filename, header);
	inFile.setSiteOnly(false);
	int numWrittenRecords = 0;
	int readIndex = -1;
	while (inFile.readRecord(record) && numWrittenRecords<numMarkers)
	{
		// work only with bi-allelic markers
		readIndex++;

		if (importIndex[readIndex])
		{
            if (numWrittenRecords % bufferSize == 0)
                {
                    printf("  Loading markers %d - %d  out of %d markers to be loaded... [%.1f%%] "
                    , LastflushPos + 1, min(LastflushPos+bufferSize,numMarkers),numMarkers,100*(double)(LastflushPos + 1)/numMarkers);
                    cout<<endl;
                }


//			 [%.1f\%]
//			std::cout << "  " << LastflushPos + 1 << "-"<<min(LastflushPos+bufferSize,numMarkers)  <<" out of " << numMarkers << " markers to be loaded..."<<endl;


            string refAllele = record.getRefStr();
			string altAllele = record.getAltStr();
			char Rallele,Aallele;

			if (strlen(refAllele.c_str()) == 1 && strlen(altAllele.c_str()) == 1)
				{
				    switch (refAllele[0])
                    {
                        case 'A': case 'a': Rallele = 1; break;
                        case 'C': case 'c': Rallele = 2; break;
                        case 'G': case 'g': Rallele = 3; break;
                        case 'T': case 't': Rallele = 4; break;
                        case 'D': case 'd': Rallele = 5; break;
                        case 'I': case 'i': Rallele = 6; break;
                        case 'R': case 'r': Rallele = 7; break;
                        default:
                        {
                                   cout << "\n\n Data Inconsistency !!! \n";
                                   cout << " Error with reference allele for marker : " << record.getIDStr() << " in VCF File : " << filename;
                                   cout << "\n VCF reference alleles for SNPs can only be A(a), C(c), G(g), or T(t).\n";
                                   cout << " " << record.getIDStr() << " has " << refAllele << endl;
                                   cout << "\n Program Aborting ... \n\n";
                                   return false;
                        }
                    }

                    switch (altAllele[0])
                    {
                        case 'A': case 'a': Aallele = 1; break;
                        case 'C': case 'c': Aallele = 2; break;
                        case 'G': case 'g': Aallele = 3; break;
                        case 'T': case 't': Aallele = 4; break;
                        case 'D': case 'd': Aallele = 5; break;
                        case 'I': case 'i': Aallele = 6; break;
                        case 'R': case 'r': Aallele = 7; break;
                        default:
                        {
                                   cout << "\n\n Data Inconsistency !!! \n";
                                   cout << " Error with alternate allele for marker : " << record.getIDStr() << " in VCF File : " << filename;
                                   cout << "\n VCF alternate alleles for SNPs can only be A(a), C(c), G(g), or T(t).\n";
                                   cout << " " << record.getIDStr() << " has " << altAllele << endl;
                                   cout << "\n Program Aborting ... \n\n";
                                   return false;
                        }
                    }


				}

			else
				{
				    Rallele = 7;
				    if(strlen(refAllele.c_str())<strlen(altAllele.c_str()))
                        Aallele=6;
                    else
                        Aallele=5;
				}

            refAlleleList.push_back(Rallele);
            VariantList[numWrittenRecords].refAllele=Rallele;
            VariantList[numWrittenRecords].altAllele=Aallele;
            VariantList[numWrittenRecords].refAlleleString=refAllele;
            VariantList[numWrittenRecords].altAlleleString=altAllele;
            haplotype_index = 0;
            for (int i = 0; i<(numSamples); i++)
            {
                if(record.getNumGTs(i)==0)
                {
                    std::cout << "\n Empty Value for Individual : " << individualName[i] << " at Marker : " << VariantList[numWrittenRecords].name << endl;
                    std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                    return false;
                }
                if(record.getNumGTs(i)==1 && finChromosome!="X")
                {
                    std::cout << "\n Single Autosomal Haplotype for Individual : " << individualName[i] << " at Marker : " << VariantList[numWrittenRecords].name << endl;
                    std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                    return false;
                }
                for (int j = 0; j<record.getNumGTs(i); j++)
                {

                    int alleleIndex = record.getGT(i, j);
                    if (alleleIndex<0)
                    {
                        std::cout << "\n Missing Value for Individual : " << individualName[i] << " at Marker : " << VariantList[numWrittenRecords].name << endl;
                        return false;
                    }

                    else
                    {
                        if(haplotype_index>=numHaplotypes && finChromosome=="X")
                        {
                            cout << "\n Error in Reference VCF File for Chromosome X !!! "<<endl;
                            cout << "\n Marker : " << VariantList[0].name << " has "<<numHaplotypes <<" haplotypes while ";
                            cout << "Marker : " << VariantList[numWrittenRecords].name << " has "<< haplotype_index+1<<" haplotypes."<<endl;
                            cout << " VCF file seems to have both Pseudo-Autosomal region (PAR) and non-PAR of chromosome X. \n";
                            cout << " Please use only either of the two regions ... \n";
                            cout << " See web-page for Minimac3 (Chromosome X Imputation) for more details ...\n";
                            cout << " Program Aborting ... "<<endl;
                            return false;
                        }
                        Haplotypes[haplotype_index]+= (char)('0'+record.getGT(i, j));
                        haplotype_index++;
                    }

                }


            }

            if(haplotype_index<(numHaplotypes-1) && finChromosome=="X")
            {
                cout << "\n Error in Reference VCF File for Chromosome X !!! "<<endl;
                cout << "\n Marker : " << VariantList[0].name << " has "<<numHaplotypes <<" haplotypes while ";
                cout << "Marker : " << VariantList[numWrittenRecords].name << " has "<< haplotype_index+1<<" haplotypes."<<endl;
                cout << " VCF file seems to have both Pseudo-Autosomal region (PAR) and non-PAR of chromosome X. \n";
                cout << " Please use only either of the two regions ... \n";
                cout << " See web-page for Minimac3 (Chromosome X Imputation) for more details ...\n";
                cout << " Program Aborting ... "<<endl;
                return false;
            }
            numWrittenRecords++;
            int length = Haplotypes[0].Length();
            vector<int> offsets(3,0);
			for (int i = 0; i < numHaplotypes; i++)
                offsets[Haplotypes[i][length - 1] - '0' + 1]++;
            offsets[2]+=offsets[1];
//            cout<<VariantList[numWrittenRecords].name<<endl;

//            int bal=*max_element(index.begin(),index.end());
//            cout<<bal<<endl;


            oldIndex = index;
            for (int i = 0; i < numHaplotypes; i++)
                {
                    index[offsets[Haplotypes[oldIndex[i]][length - 1] - '0']++] = oldIndex[i];
//                    cout<<" INDEX [ "<<i<<
                }
//            for (int i = 0; i < numHaplotypes; i++)
//                {
////                    index[offsets[Haplotypes[oldIndex[i]][length - 1] - '0']++] = oldIndex[i];
//                    cout<<" INDEX [ "<<i<<" = "<<index[i]<<endl;
//                }
//
//                bal=*max_element(oldIndex.begin(),oldIndex.end());

            RefUnique.UpdateDeltaMatrix(Haplotypes, index, firstDifference, length, blockSize,
                           oldIndex, previousPredecessor, previousDifference);
            RefUnique.AnalyzeBlocks(index, firstDifference, length, blockSize,
                       cost, bestSlice, bestComplexity, bestIndex);
//             bal=*max_element(oldIndex.begin(),oldIndex.end());
//            cout<<bal<<endl;

            if (Haplotypes[0].Length() == bufferSize)
            {
                blockedCost += RefUnique.FlushBlocks(optEndPoints,ReducedStructureInfo,VariantList,LastflushPos, Haplotypes, cost,
                                            bestComplexity, bestSlice, bestIndex);
                LastflushPos+=bufferSize;
                LastflushPos--;
                vector<String> tempHaplotypes(numHaplotypes);
                for (int i = 0; i < numHaplotypes; i++)
                {
                    tempHaplotypes[i]=Haplotypes[i][Haplotypes[i].Length()-1];
                    Haplotypes[i]=tempHaplotypes[i];
                }


                vector<int> tempoffsets(3,0);
                for (int i = 0; i < numHaplotypes; i++)
                    tempoffsets[Haplotypes[i][0] - '0' + 1]++;
                tempoffsets[2]+=tempoffsets[1];

                for (int i = 0; i < numHaplotypes; i++)
                    index[tempoffsets[Haplotypes[i][0] - '0']++] = i;




            }
        }
	}

    if (Haplotypes[0].Length() > 1)
      {
      blockedCost +=  RefUnique.FlushBlocks(optEndPoints, ReducedStructureInfo,VariantList, LastflushPos, Haplotypes, cost,
                                  bestComplexity, bestSlice, bestIndex);

//      double originalCost = 1.0 * (double) markerName.size() * (double)Haplotypes.size();

//      printf("\n After %d markers ...\n", (int)markerName.size());
//      printf("   Original cost = %.0f\n", originalCost);
//      printf("        New cost = %.0f\n", blockedCost);
//      printf("        Speed-up = %.1f\n", originalCost / (blockedCost + 1e-6));
      }

    std::cout << "\n Number of Markers Recorded                          : " << markerName.size() << endl;
    std::cout << " Number of Haplotypes Recorded                       : " << numHaplotypes << endl;

    optEndPoints.clear();
    int i;
    for(i=0;i<(int)ReducedStructureInfo.size();i++)
        {
//            cout<<i<<"\t"<<ReducedStructureInfo[i].startIndex<<"\t"<<ReducedStructureInfo[i].endIndex<<endl;
            optEndPoints.push_back(ReducedStructureInfo[i].startIndex);
        }
    optEndPoints.push_back(ReducedStructureInfo[i-1].endIndex);

    //cout<<" NOW WHAT = "<<optEndPoints.back()<<endl;

    if(individualName.size()==0)
    {
        cout << "\n No haplotypes recorded from VCF Input File : "<<filename<<endl;
		cout << " Please check the file properly..\n";
		cout << " Program Aborting ... "<<endl;
		return false;
    }

    numMarkers = markerName.size();
    std::cout << "\n Haplotype Set successfully loaded from VCF File     : " << filename << endl;
	inFile.close();

	return true;

}


bool HaplotypeSet::BasicCheckForTargetHaplotypes(String filename)
{

	VcfFileReader inFile;
	VcfHeader header;
	VcfRecord record;
	std::cout << "\n Performing basic file check on target/GWAS haplotype file : "<<filename << endl;

    std::cout << "\n Checking File ..." << endl;

	inFile.setSiteOnly(true);
    IFILE fileStream = ifopen(filename, "r");

    string line;
    if(!fileStream)
    {
		cout << "\n Program could NOT open file : " << filename << endl;
		return false;
	}
	else
    {
        std::cout << " File Exists ..." << endl;

        std::cout << "\n Checking File Type ..." << endl;

        fileStream->readLine(line);
        if(line.length()<1)
        {
            cout << "\n Target File provided by \"--haps\" must be a VCF file !!! \n";
            cout << " Please check the following file : "<<filename<<endl<<endl;
            return false;
        }

        string temp=line.substr(0,17);
        boost::to_lower(temp);

        if(temp.compare("##fileformat=vcfv")!=0)
        {
            cout << "\n Target File provided by \"--haps\" must be a VCF file !!! \n";
            cout << " Please check the following file : "<<filename<<endl<<endl;
            return false;
        }

    }

    ifclose(fileStream);

    std::cout << " VCF File Type Detected ..." << endl;



    std::cout << "\n Checking variant information ..." << endl;

    int numActualRecords=0,numReadRecords=0;
    int failFilter=0,duplicates=0,notBiallelic=0,inconsistent=0;
    string prevID,currID, refAllele,altAllele,PrefrefAllele,PrevaltAllele,cno,fixCno,id;
    inFile.open(filename, header);

    while (inFile.readRecord(record))
    {

        int flag=0;
        cno=record.getChromStr();
        int bp=record.get1BasedPosition();
        id=record.getIDStr();
        refAllele = record.getRefStr();
        altAllele = record.getAltStr();

        if(numActualRecords==0)
        {
            if(!CheckValidChrom(cno))
            {
                cout << "\n Error !!! Target VCF File contains chromosome : "<<cno<<endl;
                cout << " VCF File can only contain chromosomes 1-22 and X !!! "<<endl;
                cout << " Program Aborting ... "<<endl;
                return false;
            }

        }

        if (record.getNumAlts()>1)
		{
			notBiallelic++;
			flag = 1;
		}
		if (record.getFilter().getString(0).compare("PASS") != 0)
		{
			failFilter++;
			flag = 1;
		}
        currID=boost::lexical_cast<string>(cno)+":"+boost::lexical_cast<string>(bp);


        if(!CheckValidChrom(cno))
        {
            cout << "\n Error !!! Target VCF File contains chromosome : "<<cno<<endl;
            cout << " VCF File can only contain chromosomes 1-22 and X !!! "<<endl;
            cout << " Program Aborting ... "<<endl;
            return false;
        }
        if(fixCno!=cno && numReadRecords>0)
        {
            cout << "\n Error !!! Target VCF File contains multiple chromosomes : "<<cno<<", "<<fixCno<<", ... "<<endl;
            cout << " Please use VCF file with single chromosome !!! "<<endl;
            cout << " Program Aborting ... "<<endl;
            return false;
        }

        fixCno=cno;
        if (strlen(refAllele.c_str()) == 1 && strlen(altAllele.c_str()) == 1)
        {
            switch (refAllele[0])
            {
                case 'A': case 'a': ; break;
                case 'C': case 'c': ; break;
                case 'G': case 'g': ; break;
                case 'T': case 't': ; break;
                case 'D': case 'd': ; break;
                case 'I': case 'i': ; break;
                case 'R': case 'r': ; break;
                default:
                {
                        flag=1;
                        inconsistent++;
                }
            }
            if(flag==0)
                switch (altAllele[0])
                {
                    case '0':  ; break;
                    case 'A': case 'a': ; break;
                    case 'C': case 'c': ; break;
                    case 'G': case 'g': ; break;
                    case 'T': case 't': ; break;
                    case 'D': case 'd': ; break;
                    case 'I': case 'i': ; break;
                    case 'R': case 'r': ; break;
                    default:
                    {
                        flag=1;
                        inconsistent++;
                    }
                }
        }


        if(prevID==currID)
        {
            if(refAllele==PrefrefAllele && altAllele==PrevaltAllele)
            {
                duplicates++;
                cout << "\n Error !!! Duplicate Variant found chr:"<<boost::lexical_cast<string>(cno)+":"+boost::lexical_cast<string>(bp)<<" with identical REF = "<<refAllele <<" and ALT = "<<altAllele <<"\n";
                cout << " Program Aborting ... "<<endl;
                return false;
            }

        }

        prevID=currID;
        PrefrefAllele=refAllele;
        PrevaltAllele=altAllele;

        if(flag==0)
        {
            ++numReadRecords;
        }
        numActualRecords++;

    }


	std::cout << " "<<numActualRecords<<" variants in file with "<<numReadRecords<<" variants passing filters ..."<<endl<<endl;
    std::cout << " Checking sample information ..." << endl;

	inFile.close();

    inFile.open(filename, header);
    inFile.setSiteOnly(false);

	inFile.readRecord(record);

    std::cout << " "<<header.getNumSamples()<<" samples found in file ..."<<endl;

    if(header.getNumSamples()==0)
    {
        cout << "\n No haplotypes recorded from VCF Input File : "<<filename<<endl;
		cout << " Please check the file properly..\n";
		cout << " Program Aborting ... "<<endl;
		return false;
    }

    inFile.close();

    finChromosome=cno;

    if(finChromosome=="X")
    {
        cout<<"\n Checking Chromosome X information !!! \n";
        cout << " NOTE: All Samples in target VCF file must be either only MALE or only FEMALE ...  " << endl;

        inFile.open(filename, header);
        inFile.setSiteOnly(false);
        inFile.readRecord(record);
        int tempHapFlag=0;
        int tempHapCount=0;
        for (int i = 0; i<(header.getNumSamples()); i++)
        {
            if(record.getNumGTs(i)==0)
            {
                std::cout << "\n Empty Value for Individual at first variant !!! " << endl;
                std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                return false;
            }
            else
            {
                if(tempHapFlag!=0 && tempHapFlag!=record.getNumGTs(i))
                {
                    cout << "\n ERROR: Both haploid and diploid samples found in target VCF file "<<filename<<endl;
                    cout << " For chromosome X imputation, Males and Females should be imputed separately.\n";
                    cout << " Program Aborting ... "<<endl;
                    return false;

                }
                tempHapFlag=record.getNumGTs(i);
                tempHapCount+=record.getNumGTs(i);
            }

        }
        inFile.close();
        std::cout << " "<<tempHapCount<<" haplotypes found in file "<<endl;
    }


    std::cout << "\n Initial basic file check on target/GWAS haplotype file successful !!!" << endl<<endl;
	return true;

}


bool HaplotypeSet::LoadTargetHaplotypes(String filename, String targetSnpfile, vector<string> &refSnpList,HaplotypeSet &rHap)
{
	string FileType=DetectTargetFileType(filename);

    if(FileType.compare("mach")==0)
    {
        cout<<"\n Format = MaCH (MArkov Chain based Haplotyper) "<<endl;
        return LoadMachHaplotypes(filename, targetSnpfile, refSnpList);
    }
    else if(FileType.compare("vcf")==0)
    {
        cout<<"\n Format = VCF (Variant Call Format) "<<endl;
        return LoadVcfTargetHaplotypes(filename, targetSnpfile, refSnpList,rHap);
    }
    else if(FileType.compare("Invalid")==0)
    {

        cout << "\n Target File provided by \"--haps\" must be a VCF or MaCH file !!! \n";
        cout << " Please check the following file : "<<filename<<endl<<endl;
        return false;
    }
    else if(FileType.compare("NA")==0)
    {
        cout<<"\n Following File File Not Available : "<<filename<<endl<<endl;
        return false;
    }
    return false;
}

bool HaplotypeSet::LoadVcfTargetHaplotypes(String filename, String snpNames, vector<string> &refSnpList, HaplotypeSet &rHap)
{


	bool rsid=false;
	VcfFileReader inFile;
	VcfHeader header;
	VcfRecord record;
	vector<string> tempMarkerNames;
	tempMarkerNames.clear();

	inFile.setSiteOnly(true);
	if (!inFile.open(filename, header))
	{
		cout << "\n Program could NOT open file : " << filename << endl;
		return false;
	}
    vector<int> importIndexList;

    int numReadRecords = 0,numActualRecords=0;
    int failFilter=0,notBiallelic=0,inconsistent=0;
    std::cout << "\n Loading Target Haplotype SNP List from VCF File     : " << filename << endl<<endl;
    string cno;
    while (inFile.readRecord(record))
    {

        int flag=0;
        cno=record.getChromStr();
        int bp=record.get1BasedPosition();
        string id=record.getIDStr();
        string currID;
        string refAllele = record.getRefStr();
        string altAllele = record.getAltStr();
        char Rallele=0,Aallele=0;


        if (record.getNumAlts()>1)
		{
			notBiallelic++;
			flag = 1;
		}
		if (record.getFilter().getString(0).compare("PASS") != 0)
		{
			failFilter++;
			flag = 0;
		}
        if(rsid)
            currID=record.getIDStr();
        else
            currID=boost::lexical_cast<string>(cno)+":"+boost::lexical_cast<string>(bp);

        if (strlen(refAllele.c_str()) == 1 && strlen(altAllele.c_str()) == 1)
        {
            switch (refAllele[0])
            {
                case 'A': case 'a': Rallele = 1; break;
                case 'C': case 'c': Rallele = 2; break;
                case 'G': case 'g': Rallele = 3; break;
                case 'T': case 't': Rallele = 4; break;
                case 'D': case 'd': Rallele = 5; break;
                case 'I': case 'i': Rallele = 6; break;
                case 'R': case 'r': Rallele = 7; break;
                default:
                {
                        cout << " WARNING !!! Reference allele for SNP for "<<currID<<"is "<<refAllele<<". Will be ignored ..." <<endl;
                        flag=1;
                        inconsistent++;
                }
            }
            if(flag==0)
                switch (altAllele[0])
                {
                    case '0':  Aallele = 0; break;
                    case 'A': case 'a': Aallele = 1; break;
                    case 'C': case 'c': Aallele = 2; break;
                    case 'G': case 'g': Aallele = 3; break;
                    case 'T': case 't': Aallele = 4; break;
                    case 'D': case 'd': Aallele = 5; break;
                    case 'I': case 'i': Aallele = 6; break;
                    case 'R': case 'r': Aallele = 7; break;
                    default:
                    {
                        cout << " WARNING !!! Alternate allele for SNP for "<<currID<<"is "<<altAllele<<". Will be ignored ..." <<endl;
                        flag=1;
                        inconsistent++;
                    }
                }
        }
        else
            Rallele = 7;
        variant thisVariant(currID,cno,bp);
        if(flag==0)
        {
            VariantList.push_back(thisVariant);

            VariantList[numReadRecords].refAllele=Rallele;
            VariantList[numReadRecords].altAllele=Aallele;
            VariantList[numReadRecords].refAlleleString=refAllele;
            VariantList[numReadRecords].altAlleleString=altAllele;
            markerName.push_back(currID);
            ++numReadRecords;
        }
        numActualRecords++;
        importIndexList.push_back(flag);

    }

	std::cout << "\n Number of Markers read from VCF File                : " << numActualRecords << endl;
	std::cout << " Number of Markers with more than One Allele         : " << notBiallelic << endl;
	std::cout << " Number of Markers failing FILTER = PASS             : " << failFilter << endl;
    std::cout << " Number of Markers with inconsistent Ref/Alt Allele  : " << inconsistent << endl;

    std::cout << "\n Number of Markers to be Recorded                    : " << numReadRecords << endl;
    int refMarkerCount=(int)rHap.VariantList.size();

    finChromosome=cno;
	vector<int> knownPosition;
	int counter = 0;
	missing.resize(refMarkerCount, true);
	ScaffoldIndex.resize(refMarkerCount, -1);
    UnScaffoldIndex.resize(VariantList.size(), -1);
    AllMaleTarget=false;
//  string name;
//    int bp;
//    string chr;
//    char refAllele,altAllele;
//string refAlleleString,altAlleleString;
    int flag;
	int markerIndex=0;
	vector<string> newMarkerName;


	for (int j = 0; j<(int)VariantList.size(); j++)
	{

		int prevCounter = counter;
		flag=0;
		while(counter<refMarkerCount && flag==0 && rHap.VariantList[counter].bp<=VariantList[j].bp)
        {

            if(rHap.VariantList[counter].chr==VariantList[j].chr
             && rHap.VariantList[counter].bp==VariantList[j].bp)
            {
                prevCounter = counter;

                if(rHap.VariantList[counter].refAlleleString==VariantList[j].refAlleleString
                        && rHap.VariantList[counter].altAlleleString==VariantList[j].altAlleleString)
                    flag=1;
                else if(rHap.VariantList[counter].refAlleleString==VariantList[j].altAlleleString
                        && rHap.VariantList[counter].altAlleleString==VariantList[j].refAlleleString)
                    flag=1;
                else if (VariantList[j].refAlleleString==VariantList[j].altAlleleString
                        && rHap.VariantList[counter].refAlleleString==VariantList[j].refAlleleString)
                    flag=1;
                else if (VariantList[j].refAlleleString==VariantList[j].altAlleleString
                        && rHap.VariantList[counter].altAlleleString==VariantList[j].refAlleleString)
                    flag=1;
                else
                    counter++;
            }
            else
                counter++;
        }
        if(flag==1)
        {
            knownPosition.push_back(counter);
            newMarkerName.push_back(markerName[j]);
            missing[counter] = false;
            UnScaffoldIndex[markerIndex]=counter;
            ScaffoldIndex[counter]=markerIndex++;
            counter++;
		}
		else
        {
            knownPosition.push_back(-1);
			counter = prevCounter;
        }

	}
	numHaplotypes = 0;
			numMarkers = 0;

	std::cout << " Number of Markers overlapping with Reference List   : " << newMarkerName.size() << endl << endl;

	if (newMarkerName.size() == 0)
	{

		cout << "\n No overlap between Target and Reference markers !!!\n";
		cout << " Please check for consistent marker identifer in reference and target input files..\n";
		cout << " Program Aborting ... \n";
		return false;

	}

	markerName = newMarkerName;
    numHaplotypes = 2 * header.getNumSamples();
    numSamples=header.getNumSamples();
    if(numHaplotypes==0)
    {
        cout << "\n No haplotypes recorded from VCF Input File : "<<filename<<endl;
		cout << " Please check the file properly..\n";
		cout << " Program Aborting ... "<<endl;
		return false;
    }


    for (int i = 0; i < numSamples; i++)
	{
		string tempName(header.getSampleName(i));
		individualName.push_back(tempName);
	}

    if(finChromosome=="X")
    {
        inFile.open(filename, header);
        inFile.setSiteOnly(false);
        inFile.readRecord(record);
        int tempHapCount=0;
        for (int i = 0; i<(numSamples); i++)
        {
            if(record.getNumGTs(i)==0)
            {
                std::cout << "\n Empty Value for Individual : " << individualName[i] << " at Marker : " << VariantList[0].name << endl;
                std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                return false;
            }
            else
            {
                SampleNoHaplotypes.push_back(record.getNumGTs(i));
                tempHapCount+=record.getNumGTs(i);
            }
            if(i==0)
            {
                if(record.getNumGTs(i)==1)
                    AllMaleTarget=true;
                else
                    AllMaleTarget=false;
            }
        }
        inFile.close();
        numHaplotypes=tempHapCount;
    }



	inFile.close();

	std::cout << " Loading Target Haplotype Set from VCF File          : " << filename << endl<<endl;


	inFile.open(filename, header);
    inFile.setSiteOnly(false);

	inFile.readRecord(record);


	//haplotypes.resize(numHaplotypes);
	haplotypesUnscaffolded.resize(numHaplotypes);


	for (int i = 0; i<numHaplotypes; i++)
	{
		haplotypesUnscaffolded[i].resize(markerName.size(), 0);
    }

	// initialize this to 0 again to read each marker from each line of vcf file.
    int importIndex=-1;
    //ScaffoldIndex=knownPosition;
	int readIndex = -1;
	int numtoBeWrittenRecords = 0;
	do
	{
		// work only with bi-allelic markers
		readIndex++;
        if (importIndexList[readIndex] ==0)
		{
		    importIndex++;

            if (knownPosition[importIndex] != -1)
            {
                if (numtoBeWrittenRecords % 10000 == 0)
                       printf("  Loading markers %d out of %d markers to be loaded... [%.1f%%] \n",numtoBeWrittenRecords+1,(int)markerName.size(),100*(double)(numtoBeWrittenRecords + 1)/(int)markerName.size());

                tempMarkerNames.push_back(record.getIDStr());
                string refAllele = record.getRefStr();
                string altAllele = record.getAltStr();
                char Rallele;
                if (strlen(refAllele.c_str()) == 1 && strlen(altAllele.c_str()) == 1)
                {
                    switch (refAllele[0])
                    {
                        case 'A': case 'a': Rallele = 1; break;
                        case 'C': case 'c': Rallele = 2; break;
                        case 'G': case 'g': Rallele = 3; break;
                        case 'T': case 't': Rallele = 4; break;
                        case 'D': case 'd': Rallele = 5; break;
                        case 'I': case 'i': Rallele = 6; break;
                        case 'R': case 'r': Rallele = 7; break;
                        default:
                        {
                                   cout << "\n Data Inconsistency !!! \n";
                                   cout << " Error with reference allele for marker : " << record.getIDStr() << " in VCF File : " << filename;
                                   cout << "\n VCF reference alleles for SNPs can only be A(a), C(c), G(g), or T(t).\n";
                                   cout << " " << record.getIDStr() << " has " << refAllele << endl;
                                   cout << " Program Aborting ... \n\n";
                                   return false;
                        }
                    }
                }
                else
                    Rallele = 7;

                refAlleleList.push_back(Rallele);

                int haplotype_index = 0;
                for (int i = 0; i<(numSamples); i++)
                {

                    if(record.getNumGTs(i)==0)
                    {
                        std::cout << "\n Empty Value for Individual : " << individualName[i] << " at Marker : " <<
                         VariantList[numtoBeWrittenRecords].name << endl;
                        std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                        return false;
                    }
                    if(record.getNumGTs(i)==1 && finChromosome!="X")
                    {
                        std::cout << "\n Single Autosomal Haplotype for Individual : " << individualName[i] << " at Marker : "
                         << VariantList[numtoBeWrittenRecords].name << endl;
                        std::cout << " Most probably a corrupted VCF file. Please check input VCF file !!! " << endl;
                        return false;
                    }

//
//        bool PseudoAutosomal;
//        bool AllMaleTarget;

                    if(rHap.PseudoAutosomal)
                    {
                        if(record.getNumGTs(i)!=2)
                        {
                            cout << "\n ERROR: Reference Panel on chromosome X seems to be on Pseudo-Autosomal Region "<<endl;
                            cout <<   "        since all samples have two alleles on Chromosome X. "<<endl;
                            cout <<   "        However, "<<VariantList[numtoBeWrittenRecords].name<<" in target panel has SINGLE haplotypes !!!\n";
                            cout << " Program Aborting ... "<<endl;
                            return false;
                        }
                    }
                    else
                    {
                        if(AllMaleTarget && record.getNumGTs(i)!=1)
                        {
                            cout << "\n ERROR: Reference Panel on chromosome X seems to be on NON-Pseudo-Autosomal Region "<<endl;
                            cout <<   "        since MALE samples had single alleles on Chromosome X. "<<endl;
                            cout <<   "        However, "<<VariantList[numtoBeWrittenRecords].name<<" in target panel (all MALE samples) \n";
                            cout <<   "        has DOUBLE haplotypes !!!\n";
                            cout << " Program Aborting ... "<<endl;
                            return false;
                        }
                        if(!AllMaleTarget && record.getNumGTs(i)!=2)
                        {
                            cout << "\n ERROR: Reference Panel on chromosome X seems to be on NON-Pseudo-Autosomal Region "<<endl;
                            cout <<   "        since MALE samples had single alleles on Chromosome X. "<<endl;
                            cout <<   "        However, "<<VariantList[numtoBeWrittenRecords].name<<" in target panel (all FEMALE samples) \n";
                            cout <<   "        has SINGLE haplotypes !!!\n";
                            cout << " Program Aborting ... "<<endl;
                            return false;
                        }



                    }
                for (int j = 0; j<record.getNumGTs(i); j++)
                    {

                        int alleleIndex = record.getGT(i, j);
                        if (alleleIndex<0)
                        {
                            //std::cout << "\n Missing Value for Individual : " << individualName[i] << " at Marker : " << record.getIDStr() << endl;
                            haplotypesUnscaffolded[haplotype_index][numtoBeWrittenRecords] = 0;
                            haplotype_index++;
                            //return false;
                        }
                        else
                        {

                            const char *alleles = record.getAlleles(record.getGT(i, j));
                            char allele = convertAlleles(record.getIDStr(), individualName[i], alleles, refAllele.c_str(), altAllele.c_str());
                            //haplotypes[haplotype_index][knownPosition[readIndex]] = allele;
                            haplotypesUnscaffolded[haplotype_index][numtoBeWrittenRecords] = allele;
                            haplotype_index++;
                        }

                    }
                }
            ++numtoBeWrittenRecords;
            }
		}
	} while (inFile.readRecord(record));

	std::cout << "\n\n Number of Markers Recorded                          : " << markerName.size() << endl;
	std::cout << " Number of Haplotypes Recorded                       : " << (haplotypesUnscaffolded.size()) << endl;
    numMarkers = markerName.size();

    std::cout << "\n Haplotype Set successfully loaded from VCF File     : " << filename << endl;
	inFile.close();



	return true;

}

void HaplotypeSet::calculateFreq()
{
    alleleFreq.resize(8);
	for (int j = 0; j<8; j++)
		alleleFreq[j].resize(numMarkers, 0.0);

    int i,j,k;
    for(k=0;k<(int)ReducedStructureInfo.size();k++)
    {
        for (i = 0; i<(int)ReducedStructureInfo[k].uniqueCardinality.size(); i++)
        {
            for(j=ReducedStructureInfo[k].startIndex;j<ReducedStructureInfo[k].endIndex;j++)
            {
                alleleFreq[ReducedStructureInfo[k].uniqueHaps[i][j-ReducedStructureInfo[k].startIndex]][j]+=ReducedStructureInfo[k].uniqueCardinality[i];
            }

            if(k==(int)ReducedStructureInfo.size()-1)
                alleleFreq[ReducedStructureInfo[k].uniqueHaps[i][j-ReducedStructureInfo[k].startIndex]][j]+=ReducedStructureInfo[k].uniqueCardinality[i];
        }
    }
	major.resize(numMarkers, 0);
	minor.resize(numMarkers, 0);

//    for(k=0;k<(int)ReducedStructureInfo.size();k++)
//    {
//        for (i = 0; i<(int)ReducedStructureInfo[k].uniqueCardinality.size(); i++)
//            cout<<ReducedStructureInfo[k].uniqueCardinality[i]<<" ";
//        cout<<endl;
//    }


	for (int i = 0; i<numMarkers; i++)
	{
		double max_freq = 0.0;
		for (int j = 0; j<8; j++)
		{
			if (max_freq<alleleFreq[j][i])
			{
				max_freq = alleleFreq[j][i];
				major[i] = j;
			}
			//cout<<alleleFreq[j][i]<<"\t";
			alleleFreq[j][i] /= (double)numHaplotypes;
		}
		//cout<<endl;
		if(major[i]==VariantList[i].refAllele)
            {
                minor[i]=VariantList[i].altAllele;
                VariantList[i].MinAlleleString=VariantList[i].altAlleleString;
                VariantList[i].MajAlleleString=VariantList[i].refAlleleString;

            }

        else
            {
                minor[i]=VariantList[i].refAllele;
                VariantList[i].MinAlleleString=VariantList[i].refAlleleString;
                VariantList[i].MajAlleleString=VariantList[i].altAlleleString;
            }


	}

}

char HaplotypeSet::getScaffoldedHaplotype(int sample,int marker)
{

   if(missing[marker]==true)
        return 0;
   else
        return haplotypesUnscaffolded[sample][ScaffoldIndex[marker]];
}



bool HaplotypeSet::LoadSnpList(String filename)
{

	std::cout << "\n Loading Marker List from File                       : " << filename << endl;

    if(filename=="")
    {
        cout<<"\n No File for Marker List found !!!! \n";
        cout<<" Please use \"--snps\" parameter if \"--haps\" is a MaCH file ... \n";
        cout<<endl;
        return false;
    }

    IFILE ifs = ifopen(filename, "r");
    string line;
    if(ifs)
    {
        while ((ifs->readLine(line))!=-1)
        {
            markerName.push_back(line);
            line.clear();

        }
    }
    else
    {
        cout<<"\n Following File File Not Available : "<<filename<<endl;
        return false;
    }

	ifclose(ifs);
	return true;

}


bool HaplotypeSet::LoadMachHaplotypes(String filename, String targetSnpfile, vector<string> &refSnpList)
{
	typedef boost::tokenizer< boost::char_separator<char> > wsTokenizer;

	std::cout << "\n Loading Target Haplotype Set from MaCH File         : " << filename << endl;


	if(!LoadSnpList(targetSnpfile))
        return false;

	vector<int> knownPosition;
	int counter = 0;

	missing.resize(refSnpList.size(), true);

	vector<string> newMarkerName;
    ScaffoldIndex.resize(refSnpList.size(), -1);
    int markerIndex=0;
	for (int j = 0; j<(int)markerName.size(); j++)
	{
		int prevCounter = counter;
		while (counter<(int)refSnpList.size() && refSnpList[counter].compare(markerName[j]) != 0)
		{
			counter++;
		}
		if (counter == (int)refSnpList.size())
		{
			knownPosition.push_back(-1);
			counter = prevCounter;
		}
		else
		{
			knownPosition.push_back(counter);
			newMarkerName.push_back(markerName[j]);
			missing[counter] = false;
			ScaffoldIndex[counter]=markerIndex++;

		}
	}

	std::cout << "\n Number of Markers in Data                           : " << markerName.size() << endl;
	std::cout << " Number of Markers overlapping with Reference List   : " << newMarkerName.size() << endl << endl;

	if (newMarkerName.size() == 0)
	{

		cout << "\n No overlap between Target and Reference markers !!!\n";
		cout << " Please check for consistent marker identifer in reference and target input files..\n";
		cout << " Program Aborting ... \n";
		return false;

	}

	markerName = newMarkerName;

	int n = 0;

	IFILE ifs = ifopen(filename, "r");


	string line;
	if(ifs)
    {
        while ((ifs->readLine(line))!=-1)
        {
            n++;
            boost::char_separator<char> sep(" \t");
            wsTokenizer t(line, sep);
            wsTokenizer::iterator i;
            int col = 0;

            for (i = t.begin(); i != t.end(); ++i)
            {
                col++;

                if (col == 1)
                {

                    string tempInd;

                    tempInd = i->c_str();
                    if ((n-1) % 2 == 0)
                    {
                        individualName.push_back(tempInd);
                    }
                }
                if (col == 3)
                {
                    string tempHap;
                    tempHap = i->c_str();
                    //vector<char> tempSnp(refSnpList.size(), 0);
                    //cout<<line<<endl;

                    vector<char> tempSnp2(markerName.size(), 0);
                    int index=0;
                    for (int j = 0; j<(int)knownPosition.size() ;  j++)
                    {
                        if (knownPosition[j] != -1)
                        {
                            char allele=0;
                            switch (tempHap[j])
                            {
                            case 'A': case 'a': case '1': allele = 1; break;
                            case 'C': case 'c': case '2': allele = 2; break;
                            case 'G': case 'g': case '3': allele = 3; break;
                            case 'T': case 't': case '4': allele = 4; break;
                            case 'D': case 'd': case '5': allele = 5; break;
                            case 'I': case 'i': case '6': allele = 6; break;
                            case 'R': case 'r': case '7': allele = 7; break;
                            default:
                                if (!allowMissing)
                                {

                                    cout << " Error: Haplotypes can only contain alleles A ('A', 'a' or '1'),\n";
                                    cout << " C ('C', 'c' or 2), G ('G', 'g' or 3), T ('T', 't' or '4'),\n";
                                    cout << " D ('D', 'd' or 5), I ('I', 'i' or 6) and R ('R', 'r' or 7).\n";
                                    cout << "\n\n For Individual : " << individualName.back() << ", Haplotype #";
                                    cout << (n-1) % 2 +1 << " has allele \"" << tempHap[j] << "\" at position : ";
                                    cout << j + 1 << " in MaCH input file <" << filename << ">" << endl;


                                    return false;
                                }
                            }

                            tempSnp2[index++] = allele;
                            //tempSnp[knownPosition[j]] = allele;
                        }


                    }
                haplotypesUnscaffolded.push_back(tempSnp2);
                    //haplotypes.push_back(tempSnp);
			}
            }
            line.clear();
        }
    }
    else
    {
        cout<<"\n\n Following File File Not Available : "<<filename<<endl;
        return false;
    }

	std::cout << " Number of Markers Recorded                          : " << markerName.size() << endl;
	std::cout << " Number of Haplotypes Recorded                       : " << (haplotypesUnscaffolded.size()) << "\n";
    numHaplotypes = haplotypesUnscaffolded.size();
    numMarkers = markerName.size();


    if(haplotypesUnscaffolded.size()==0)
    {
        cout << "\n No haplotypes recorded from MaCH Input File : "<<filename<<endl;
		cout << " Please check the file properly..\n";
		cout << " Program Aborting ... "<<endl;
		return false;
    }
	std::cout << "\n Haplotype Set successfully loaded from MaCH File    : " << filename << endl;
	ifclose(ifs);

	return true;
}


char HaplotypeSet::convertAlleles(string markerId, string indivId, const char *alleles, string refAlleleString, string altAlleleString)
{

	char allele;

	if (strlen(refAlleleString.c_str()) == 1 && strlen(altAlleleString.c_str()) == 1)
		switch (alleles[0])
	{
		case 'A': case 'a': allele = 1; break;
		case 'C': case 'c': allele = 2; break;
		case 'G': case 'g': allele = 3; break;
		case 'T': case 't': allele = 4; break;
		case 'D': case 'd': allele = 5; break;
		case 'I': case 'i': allele = 6; break;
		case 'R': case 'r': allele = 7; break;
		default:
		{
				   cout << "\n Data Inconsistency !!! \n";
				   cout << " Error with alleles for Marker : " << markerId << " and Individual : ";
				   cout << indivId << " in Reference VCF File. " << endl;
				   cout << " Alleles for SNPs can only be A(a), C(c), G(g), or T(t).\n";
				   cout << " Genotype of Individual : " << indivId << " at marker " << markerId << " is " << alleles << endl;
				   cout << " Program Aborting ... \n\n";
				   abort();
		}
	}
	else
	{
		string tempAllele(alleles);
		if (strlen(refAlleleString.c_str())<strlen(altAlleleString.c_str()))
		{
			if (refAlleleString.compare(tempAllele) == 0)
				allele = 7;
			else if (altAlleleString.compare(tempAllele) == 0)
				allele = 6;
			else
			{
				cout << "\n Data Inconsistency !!! \n";
				cout << " Genotype of Individual : " << indivId << " at marker " << markerId << " is " << alleles << endl;
				cout << " It does NOT match the Reference Allele : " << refAlleleString << " nor the Alternate Allele : " << altAlleleString << endl;
				cout << " Program supports only bi-allelic markers \n";
				cout << " Program Aborting ... \n\n";
				abort();
			}


		}
		else
		{
			if (refAlleleString.compare(tempAllele) == 0)
				allele = 7;
			else if (altAlleleString.compare(tempAllele) == 0)
				allele = 5;
			else
			{
				cout << "\n Data Inconsistency !!! \n";
				cout << " Genotype of Individual : " << indivId  << " at marker " << markerId << " is " << alleles << endl;
				cout << " It does NOT match the Reference Allele : " << refAlleleString << " nor the Alternate Allele : " << altAlleleString << endl;
				cout << " Program supports only bi-allelic markers \n";
				cout << " Program Aborting ... \n\n";
				abort();
			}


		}


	}


	return allele;




}






