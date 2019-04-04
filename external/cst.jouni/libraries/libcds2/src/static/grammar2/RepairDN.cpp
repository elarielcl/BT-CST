#include <grammar/Repair.h>
#include <utils/libcdsBasics.h>
#include <stdexcept>

using namespace cds_utils;

namespace cds_static {

    RepairDN::RepairDN(char *input, long len, uint MB) {
        runRepair(input, len, MB);
    }

    RepairDN::RepairDN(int *input, long len, uint MB) {
        runRepair(input, len, MB);
    }

    Repair *RepairDN::instanceFromBitmap(uint *input, long len, uint MB) {
        RepairDN *rp = new RepairDN();
        int *int_input = new int[len];
        for (uint i=0;i<len;i++) int_input[i] = (bitget(input,i))?1:0;
        rp->runRepair(int_input,len,MB);
        delete [] int_input;
        return rp;
    }

    RepairDN::RepairDN() {

    }

    void RepairDN::runRepair(char *input, long len, uint MB) {
        input_len=len;
        //TODO
        uint *inputint = (uint*)malloc(sizeof(len));
        C = (long *)malloc(sizeof(long)*len);
        uint i=0;
        for (i=0;i<len;i++){
            C[i]=(int)input[i];
            inputint[i]=(int)input[i];
        }
        diskRead=diskWrite=did0=0;
        char *filename1;
        char *filename2;
        prepare0(len,filename1);
        repair0(len, filename1, filename2);
        prepare(len);
        repair();
        postRepair();
        statistics();
        //~ printGrammar();
        grammarDepth=-1;
        assert(testRepair(inputint));
    }

    void RepairDN::runRepair(int *input, long len, uint MB) {
        input_len=len;
        uint *inputint = (uint*)malloc(sizeof(uint)*len);
        C = (long *)malloc(sizeof(long)*len);
        uint i;
        for (i=0;i<len;i++){
            C[i]=input[i];
            inputint[i]=input[i];
        }
        diskRead=diskWrite=did0=0;

        char *filename1;
        char *filename2;
        prepare0(len,filename1);
        repair0(len, filename1, filename2);
        prepare(len);
        repair();
        postRepair();
        statistics();
        //~ printGrammar();
        grammarDepth=-1;
        assert(testRepair(inputint));
    }

    void RepairDN::prepare0(long len, char *filename) {
        long i,j;
        int id;
        Tpair_repair pair;
        alph = 0;
        long nb = nblocks(len,BSI);
        Rec = createRecords(factor,minsize);
        Heap = createHeap(len,&Rec,factor,minsize);
        Hash = createHash(256*256,&Rec);
        assocRecords (&Rec,&Hash,&Heap,NULL);
        FILE *file = fopen(filename,"rb");
        if (file==NULL){
            fprintf(stderr,"Error opening input file at prepare0.\n");
            abort();
        }
        uint  prev;
        did0 = 1;
        for (j=0;j<nb;j++){
            size_t read = readBlock(file,buffer);
            long ini,fin;
            ini = 0;
            fin = ini+ read;
            if (j==0){
                prev = buffer[0];
                ini = 1;
                alph = buffer[0];
            }
            for (i=ini;i<fin;i++){
                alph = max(buffer[i],(uint)alph);
                pair.left  = prev;
                pair.right = buffer[i];
                prev = buffer[i];
                id = searchHash (Hash,pair);
                if (id == -1){// new pair, insert
                    id = insertRecord (&Rec,pair);
                }else{
                    incFreq (&Heap,id);
                }
                if (PRNL && (j%1000000 == 0)) printf ("Processed %lli blocks\n",j);
            }
        }
        n = ++alph;
        //n is the first NON-TERMINAL
        fclose(file);
        purgeHeap (&Heap);
    }

    void RepairDN::save(ofstream &fp){
        uint w = REPAIRDN_HDR;
        saveValue(fp,w);
        saveInternal(fp);
    }

    Repair *RepairDN::load(ifstream &fp){
        uint w;
        w = loadValue<uint>(fp);
        if (w!=REPAIRDN_HDR){
            return nullptr;
        }
        Repair *rp = new RepairDN();
        Repair::loadInternal(fp  , rp);
        return rp;
    }

    long RepairDN::repair0(long len, char *filename1, char *filename2){
        int oid,id;
        long cpos,pos;
        Trecord_repair *rec,*orec;
        Tpair_repair pair;
        int left,right;
        RULES_LEFT.reserve(MAX_RULES);
        RULES_RIGHT.reserve(MAX_RULES);
        FREQ_PAIRS.reserve(MAX_RULES);

        bufferWrite = (uint*) malloc(BSB);
        //if (fwrite(&alph,sizeof(int),1,R) != 1) return -1;
        if (PRNL) printf ("--- first stage, n = %lli\n",len);
        //if (PRNC) prntext(len);

        uint tailPrevBlock[2];
        //uint *writeStream = (uint*)malloc(sizeof(uint)*len);
        uint valuePrevPos = -1;
        uint valueCpos=buffer[0];
        uint valueCpos1=buffer[1];
        uint valueCpos2=buffer[2];
        uint iter = 0;

        char *file_name[2];
        file_name[0] = filename1;
        file_name[1] = filename2;
        FILE *files[2];
        files[0]=files[1]=NULL;

        while (n+1 > 0){
            //files[0] = read stream; files[1] = write stream;
            files[0] = fopen(file_name[0],"rb");
            files[1] = fopen(file_name[1],"wb");
            if (files[0]==NULL || files[1]==NULL){
                printf("Error opening files in repair0\n");
                abort();
            }

            readBlock(files[0],buffer);
            valueCpos1 =buffer[0];
            valueCpos2 =buffer[1];

            if ((iter++)>=20){
                fclose(files[0]);
                fclose(files[1]);
                break;
            }else{
                if (PRNP)
                    printf ("Avoiding to use %lli MB\n",
                            (len /1024/1024) * 3 * sizeof(long));
            }
            if (PRNR) prnRec();

            oid = extractMax(&Heap);
            if (oid == -1){
                fclose(files[0]);
                fclose(files[1]);
                break;// the end!!
            }

            orec = &Rec.records[oid];
//            if (fwrite (&orec->pair,sizeof(Tpair),1,R) != 1){
//                fprintf(stderr,"Error while writing Tpair\n");
//                abort();
//            }
            RULES_LEFT.push_back(getSym(orec->pair.left));
            RULES_RIGHT.push_back(getSym(orec->pair.right));
            FREQ_PAIRS.push_back(orec->freq);
            rule_index++;

            left  = orec->pair.left;
            right = orec->pair.right;

            if (PRNP){
                printf("Chosen pair %i = (",n);
                prnSym(orec->pair.left);
                printf(",");
                prnSym(orec->pair.right);
                printf(") (%lli occs)\n",orec->freq);
            }
            pos = 0;//pos is the writing pointer
            for (cpos=0;cpos<len-1;cpos++){
                assert(pos<=cpos);
                readInBuffer(files[0],buffer,tailPrevBlock,BSI,cpos,&valueCpos,&valueCpos1,&valueCpos2);

                if ((valueCpos != left) || (valueCpos1 != right)){
                    valuePrevPos = valueCpos;
                    writeInBuffer(files[1],bufferWrite,BSI,pos,valueCpos);
                }else{ // occurrence of the pair to modify

                    // decr freqs of pairs that disappear
                    if (pos > 0){
                        pair.left = valuePrevPos;//text[pos-1];
                        pair.right = valueCpos;
                        id = searchHash(Hash,pair);
                        if (id != -1){ // may not exist if purgeHeap'd
                            if (id != oid)
                                decFreq(&Heap,id); //not to my pair!
                        }
                    }

                    if (cpos < len-2){
                        pair.left = valueCpos1;
                        pair.right = valueCpos2;
                        id = searchHash(Hash,pair);
                        if (id != -1){// may not exist if purgeHeap'd
                            if (id != oid)
                                decFreq(&Heap,id); //not to my pair!
                        }
                    }

                    // create or incr freqs of pairs that appear
                    if (pos > 0){
                        pair.left = valuePrevPos;//text[pos-1];
                        pair.right = n;
                        id = searchHash(Hash,pair);
                        if (id == -1){ // new pair, insert
                            id = insertRecord (&Rec,pair);
                        }else{
                            incFreq (&Heap,id);
                        }
                    }

                    if (cpos < len-2){
                        pair.left = n;
                        pair.right = valueCpos2;
                        id = searchHash(Hash,pair);
                        if (id == -1){// new pair, insert
                            id = insertRecord (&Rec,pair);
                        }else{
                            incFreq (&Heap,id);
                        }
                    }
                    // actually do the substitution
                    valuePrevPos = n;
                    writeInBuffer(files[1],bufferWrite,BSI,pos,n);
                    cpos++;
                }
                pos++;
            }
            assert(pos<=cpos);
            readInBuffer(files[0],buffer,tailPrevBlock,BSI,cpos,&valueCpos,&valueCpos1,&valueCpos2);
            if (cpos == len-1){
                writeInBuffer(files[1],bufferWrite,BSI,pos,valueCpos);
                pos++;
            }
            len = pos;
            writeBlock(files[1],bufferWrite);//flush the buffer
            fclose(files[1]);
            fclose(files[0]);

           // if (PRNC) prntext(len);
            removeRecord (&Rec,oid);
            n++;
            purgeHeap(&Heap); // remove freq 1 from heap
            //swap files: now i read that I wrote and vice versa
            char *swp = file_name[0];
            file_name[0] = file_name[1];
            file_name[1] = swp;

        }//end while(n+1>0)
        purgeHeap(&Heap); // remove freq 1 from heap, if it exited for oid=-1
        files[0] = fopen(file_name[0],"rb");
        //TODO
        throw new runtime_error("Load the input into C is still missing");

//        text = (uint*)malloc(sizeof(uint)*len);
//        fread(text,sizeof(uint),len,files[0]);
        free(bufferWrite);
        return len;
    }

    long RepairDN::readBlock(FILE *in, uint *b){
        diskRead++;
        return fread(b,sizeof(uint),BSI,in);
    }

    long RepairDN::writeBlock(FILE *in, uint *b){
        diskWrite++;
        return fwrite(b,sizeof(uint),BSI,in);
    }

    long RepairDN::rewindFile(FILE *in){
        rewind(in);
    }

    //bs is the block size in bytes
    long RepairDN::nblocks(unsigned long long len, unsigned long long bs){
        return len/bs + ((len%bs)?1:0);
    }

    //writes at pos%capacity of the buffer the value w. If pos is at the end of the
//buffer, it is written in f.
    long RepairDN::writeInBuffer(FILE *f, uint *buffer, size_t capacity, size_t pos, uint w){
        size_t p = pos%capacity;
        buffer[p]=w;

        if ((p+1)==capacity){
            writeBlock(f,buffer);
            p=0;
        }

        return p+1;
    }

    long RepairDN::readInBuffer(FILE *f, uint *buffer, uint *tailPrev, size_t capacity, size_t pos, uint *valueCpos, uint *valueCpos1, uint *valueCpos2){
        uint newRead;
        size_t offset = (pos+2)%capacity;

        if (offset==0){
            (*valueCpos)  = tailPrev[0]=buffer[capacity-2];
            (*valueCpos1) = tailPrev[1]=buffer[capacity-1];
            readBlock(f,buffer);
            (*valueCpos2) = buffer[0];
        }else if (offset==1){
            (*valueCpos)  = tailPrev[1];
            (*valueCpos1) = buffer[0];
            (*valueCpos2) = buffer[1];
        }else{
            (*valueCpos2) = buffer[offset  ];
            (*valueCpos1) = buffer[offset-1];
            (*valueCpos)  = buffer[offset-2];
        }

        return pos;
    }

    void RepairDN::prepare(long len) {
        long i;
        int id;
        Tpair_repair pair;
        c = u = len;
        L = (Tlist_repair*)malloc(u*sizeof(Tlist_repair));
        assocRecords (&Rec,&Hash,&Heap,L);
        for (i=0;i<c-1;i++)
        { pair.left = C[i]; pair.right = C[i+1];
            id = searchHash (Hash,pair);
            if (id == -1) // non existing => occurs only once, don't create
            { if (did0) L[i].prev = NullFreq;
                else { id = insertRecord (&Rec,pair);
                    L[i].prev = -id-1;
                    Rec.records[id].cpos = i;
                }
                L[i].next = -1;
            }
            else
            { if (Rec.records[id].cpos == -1) // first time I see it this pass
                { L[i].next = -1;
                }
                else
                { L[i].next = Rec.records[id].cpos;
                    L[L[i].next].prev = i;
                    if (!did0) incFreq (&Heap,id);
                }
                L[i].prev = -id-1;
                Rec.records[id].cpos = i;
            }
            if (PRNL && (i%1000000 == 0)) printf ("Processed %lli ints\n",i);
        }
        L[i].prev = NullFreq; L[i].next = -1;
        purgeHeap (&Heap);
    }

    void RepairDN::repair() {
        int oid,id;
        long cpos;
        Trecord_repair *rec,*orec;
        Tpair_repair pair;
        if (PRNL) printf ("--- third stage, n=%lli\n",c);
        if (PRNC) prnC();
        while (n+1 > 0)
        {
            if (PRNR) prnRec();
            oid = extractMax(&Heap);
            if (oid == -1) break; // the end!!
            orec = &Rec.records[oid];
            cpos = orec->cpos;
            RULES_LEFT.push_back(getSym(orec->pair.left));
            RULES_RIGHT.push_back(getSym(orec->pair.right));
            FREQ_PAIRS.push_back(orec->freq);
            rule_index++;

            //if (fwrite (&orec->pair,sizeof(Tpair),1,R) != 1) return -1;
            if (PRNP)
            { printf("Chosen pair %i = (",n);
                prnSym(orec->pair.left);
                printf(",");
                prnSym(orec->pair.right);
                printf(") (%lli occs)\n",orec->freq);
            }
            while (cpos != -1)
            { long ant,sgte,ssgte;
                // replacing bc->e in abcd, b = cpos, c = sgte, d = ssgte
                if (C[cpos+1] < 0) sgte = -C[cpos+1]-1;
                else sgte = cpos+1;
                if ((sgte+1 < u) && (C[sgte+1] < 0)) ssgte = -C[sgte+1]-1;
                else ssgte = sgte+1;
                // remove bc from L
                if (L[cpos].next != -1) L[L[cpos].next].prev = -oid-1;
                orec->cpos = L[cpos].next;
                if (ssgte != u) // there is ssgte
                { 	// remove occ of cd
                    pair.left = C[sgte]; pair.right = C[ssgte];
                    id = searchHash(Hash,pair);
                    if (id != -1) // may not exist if purgeHeap'd
                    { if (id != oid) decFreq (&Heap,id); // not to my pair!
                        if (L[sgte].prev != NullFreq) //still exists(not removed)
                        { rec = &Rec.records[id];
                            if (L[sgte].prev < 0) // this cd is head of its list
                                rec->cpos = L[sgte].next;
                            else L[L[sgte].prev].next = L[sgte].next;
                            if (L[sgte].next != -1) // not tail of its list
                                L[L[sgte].next].prev = L[sgte].prev;
                        }
                    }
                    // create occ of ed
                    pair.left = n;
                    id = searchHash(Hash,pair);
                    if (id == -1) // new pair, insert
                    { id = insertRecord (&Rec,pair);
                        rec = &Rec.records[id];
                        L[cpos].next = -1;
                    }
                    else
                    { incFreq (&Heap,id);
                        rec = &Rec.records[id];
                        L[cpos].next = rec->cpos;
                        L[L[cpos].next].prev = cpos;
                    }
                    L[cpos].prev = -id-1;
                    rec->cpos = cpos;
                }
                if (cpos != 0) // there is ant
                { 	// remove occ of ab
                    if (C[cpos-1] < 0)
                    { ant = -C[cpos-1]-1;
                        if (ant == cpos) // sgte and ant clashed -> 1 hole
                            ant = cpos-2;
                    }
                    else ant = cpos-1;
                    pair.left = C[ant]; pair.right = C[cpos];
                    id = searchHash(Hash,pair);
                    if (id != -1) // may not exist if purgeHeap'd
                    { if (id != oid) decFreq (&Heap,id); // not to my pair!
                        if (L[ant].prev != NullFreq) //still exists (not removed)
                        { rec = &Rec.records[id];
                            if (L[ant].prev < 0) // this ab is head of its list
                                rec->cpos = L[ant].next;
                            else L[L[ant].prev].next = L[ant].next;
                            if (L[ant].next != -1) // it is not tail of its list
                                L[L[ant].next].prev = L[ant].prev;
                        }
                    }
                    // create occ of ae
                    pair.right = n;
                    id = searchHash(Hash,pair);
                    if (id == -1) // new pair, insert
                    { id = insertRecord(&Rec,pair);
                        rec = &Rec.records[id];
                        L[ant].next = -1;
                    }
                    else
                    { incFreq (&Heap,id);
                        rec = &Rec.records[id];
                        L[ant].next = rec->cpos;
                        L[L[ant].next].prev = ant;
                    }
                    L[ant].prev = -id-1;
                    rec->cpos = ant;
                }
                C[cpos] = n;
                if (ssgte != u) C[ssgte-1] = -cpos-1;
                C[cpos+1] = -ssgte-1;
                c--;
                orec = &Rec.records[oid]; // just in case of Rec.records realloc'd
                cpos = orec->cpos;
            }
            if (PRNC) prnC();
            removeRecord (&Rec,oid);
            n++;
            purgeHeap(&Heap); // remove freq 1 from heap
            if (c < factor * u) // compact C
            { int i,ni;
                i = 0;
                for (ni=0;ni<c-1;ni++)
                { C[ni] = C[i];
                    L[ni] = L[i];
                    if (L[ni].prev < 0)
                    { if (L[ni].prev != NullFreq) // real ptr
                            Rec.records[-L[ni].prev-1].cpos = ni;
                    }
                    else L[L[ni].prev].next = ni;
                    if (L[ni].next != -1) L[L[ni].next].prev = ni;
                    i++; if (C[i] < 0) i = -C[i]-1;
                }
                C[ni] = C[i];
                u = c;
                C = (long*)realloc (C, c * sizeof(long));
                L = (Tlist_repair*)realloc (L, c * sizeof(Tlist_repair));
                assocRecords (&Rec,&Hash,&Heap,L);
            }
        }
    }
}