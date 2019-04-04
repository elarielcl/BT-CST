/* Copyright (C) 2010, Rodrigo Cánovas, all rights reserved.
 *
 *This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <LCP_FMN_RLCSA.h>
#include <LCP_SAD.h>

int lcp_count;
namespace cds_static{

	LCP_FMN_RLCSA::LCP_FMN_RLCSA(){
		lcp_type = FMN_RRR_OS;
		O = NULL;
		Z = NULL;
		b_length = OZ_type = 0;
		lcp_count = 0;
	}

	void LCP_FMN_RLCSA::generate_OZ(BitSequence *U, uint **O, uint **Z, uint length){
		uint *o, *z;
		bool bit_1 = true; //tell if we are counting 1 bits or 0 bits
		uint cont_1 = 0;
		uint cont_0 = 0;
		long long nb = 1;
		nb = (nb*length+W-1)/W;
		if(nb > MAXINT){
			cout << "Memory limit excess (in LCP)" << endl;
			exit(1);
		}
		o = new uint[(uint)nb];
		z = new uint[(uint)nb];

		for(uint i=0; i< (uint)nb; i++){
			o[i]=0;
			z[i]=0;
		}

		for(uint i=0; i<length; i++){
			if( U->access(i)){
				if(bit_1)
					cont_1++;
				else{
					bit_1 = true;
					cont_1++;
					bitset(z,cont_0-1);
				}
			}
			else{
				if(bit_1){
					bit_1 = false;
					cont_0++;
					bitset(o,cont_1-1);
				}
				else
					cont_0++;
			}
		}
		if(bit_1)
			bitset(o,cont_1-1);
		else
			bitset(z,cont_0-1);
		*O = o;
		*Z = z;
	}

	LCP_FMN_RLCSA::LCP_FMN_RLCSA(TextIndex *csa, char *text, size_t n, size_t op_rs){
		if(op_rs!=RRR02_HDR && op_rs!=SDARRAY_HDR && op_rs!=DELTA){
			cout << "Error: op_rs must be RRR02_HDR or SDARRAY_HDR\n" << endl;
			exit(1);
		}
		lcp_type = FMN_RRR_OS;
		OZ_type = op_rs;
		LCP_SAD *sa_lcp = new LCP_SAD(csa, text, n);
		uint *O_aux, *Z_aux;
		b_length = sa_lcp->U_length;
		generate_OZ(sa_lcp->U, &O_aux, &Z_aux, b_length);
		 
		lcp_count=0;

                if(op_rs==DELTA){
                        O = new BitSequenceRRR(O_aux, b_length);
                        Z = new BitSequenceRRR(Z_aux, b_length);
                        uint *Obit=new uint[b_length];
                        for(int i=0; i<b_length; ++i)
                            Obit[i]=0;

                        uint aux2, l1,l2;
                        uint aux=O->select1(1);
                        Obit[0]= aux+1;
                         for(l1=2; l1<b_length; ++l1){
                             aux2=O->select1(l1);
                             if(aux2-aux){//>0 
                                Obit[l1-1]= aux2-aux;
                                aux=aux2;
                             }
                             else break;
                         }
                         cout << "l1:" <<l1 << endl;

                        aux=Z->select1(1);
                        Obit[0]= aux+1;
                         for(l2=2; l2<b_length; ++l2){
                             aux2=Z->select1(l2);
                             if(aux2-aux){
                                Obit[l2-1]= aux2-aux;
                                aux=aux2;
                             }
                             else break;
                         }
                        cout << "l2:" <<l2 << endl;
			Z = new BitSequenceDelta(Obit, l2);

		}
                else if(op_rs==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);

		}
		else{
			O = new BitSequenceSDArray(O_aux, b_length);
			Z = new BitSequenceSDArray(Z_aux, b_length);
		}
		delete (LCP_SAD *)sa_lcp;
		delete [] O_aux;
		delete [] Z_aux;

                //for(int i=0; i<b_length; ++i)
                  //  cout <<"i: "<< i << " O[i]: " << O->select1(i) << endl;

	}
	LCP_FMN_RLCSA::LCP_FMN_RLCSA(RLCSA *csa, char *text, size_t n, size_t op_rs){
		if(op_rs!=RRR02_HDR && op_rs!=SDARRAY_HDR && op_rs!=DELTA){
			cout << "Error: op_rs must be RRR02_HDR or SDARRAY_HDR\n" << endl;
			exit(1);
		}
		lcp_type = FMN_RRR_OS;
		OZ_type = op_rs;
		LCP_SAD *sa_lcp = new LCP_SAD(csa, text, n);
		uint *O_aux, *Z_aux;
		b_length = sa_lcp->U_length;
		generate_OZ(sa_lcp->U, &O_aux, &Z_aux, b_length);

		lcp_count=0;

		if(op_rs==DELTA){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);
			uint *Obit=new uint[b_length];
			for(int i=0; i<b_length; ++i)
				Obit[i]=0;

			uint aux2, l1,l2;
			uint aux=O->select1(1);
			Obit[0]= aux+1;
			for(l1=2; l1<b_length; ++l1){
				aux2=O->select1(l1);
				if(aux2-aux){//>0
					Obit[l1-1]= aux2-aux;
					aux=aux2;
				}
				else break;
			}
			//cout << "l1:" <<l1 << endl;

			aux=Z->select1(1);
			Obit[0]= aux+1;
			for(l2=2; l2<b_length; ++l2){
				aux2=Z->select1(l2);
				if(aux2-aux){
					Obit[l2-1]= aux2-aux;
					aux=aux2;
				}
				else break;
			}
			//cout << "l2:" <<l2 << endl;
			Z = new BitSequenceDelta(Obit, l2);

		}
		else if(op_rs==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);

		}
		else{
			O = new BitSequenceSDArray(O_aux, b_length);
			Z = new BitSequenceSDArray(Z_aux, b_length);
		}
		delete (LCP_SAD *)sa_lcp;
		delete [] O_aux;
		delete [] Z_aux;

		//for(int i=0; i<b_length; ++i)
		//  cout <<"i: "<< i << " O[i]: " << O->select1(i) << endl;

	}
	LCP_FMN_RLCSA::LCP_FMN_RLCSA(TextIndex *csa, uint *lcp_verb, char *text, size_t n, size_t op_rs){
		if(op_rs!=RRR02_HDR && op_rs!=SDARRAY_HDR && op_rs!=DELTA){
			cout << "Error: op_rs must be RRR02_HDR or SDARRAY_HDR\n" << endl;
			exit(1);
		}
		lcp_type = FMN_RRR_OS;
		OZ_type = op_rs;
		LCP_SAD *sa_lcp = new LCP_SAD(csa, lcp_verb, text, n);
		// for (uint i=0;i<n;i++){
		// 	assert(sa_lcp->get_LCP(i, csa)==lcp_verb[i]);
		// }
		uint *O_aux, *Z_aux;
		b_length = sa_lcp->U_length;
		generate_OZ(sa_lcp->U, &O_aux, &Z_aux, b_length);
		 
		lcp_count=0;

                if(op_rs==DELTA){
                        O = new BitSequenceRRR(O_aux, b_length);
                        Z = new BitSequenceRRR(Z_aux, b_length);
                        uint *Obit=new uint[b_length];
                        for(int i=0; i<b_length; ++i)
                            Obit[i]=0;

                        uint aux2, l1,l2;
                        uint aux=O->select1(1);
                        Obit[0]= aux+1;
                         for(l1=2; l1<b_length; ++l1){
                             aux2=O->select1(l1);
                             if(aux2-aux){//>0 
                                Obit[l1-1]= aux2-aux;
                                aux=aux2;
                             }
                             else break;
                         }
                         cout << "l1:" <<l1 << endl;

                        aux=Z->select1(1);
                        Obit[0]= aux+1;
                         for(l2=2; l2<b_length; ++l2){
                             aux2=Z->select1(l2);
                             if(aux2-aux){
                                Obit[l2-1]= aux2-aux;
                                aux=aux2;
                             }
                             else break;
                         }
                        cout << "l2:" <<l2 << endl;
			Z = new BitSequenceDelta(Obit, l2);

		}
                else if(op_rs==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);

		}
		else{
			O = new BitSequenceSDArray(O_aux, b_length);
			Z = new BitSequenceSDArray(Z_aux, b_length);
		}

		delete (LCP_SAD *)sa_lcp;
		delete [] O_aux;
		delete [] Z_aux;

                //for(int i=0; i<b_length; ++i)
                  //  cout <<"i: "<< i << " O[i]: " << O->select1(i) << endl;

	}
	LCP_FMN_RLCSA::LCP_FMN_RLCSA(RLCSA *csa, uint *lcp_verb, char *text, size_t n, size_t op_rs){
		if(op_rs!=RRR02_HDR && op_rs!=SDARRAY_HDR && op_rs!=DELTA){
			cout << "Error: op_rs must be RRR02_HDR or SDARRAY_HDR\n" << endl;
			exit(1);
		}
		lcp_type = FMN_RRR_OS;
		OZ_type = op_rs;
		LCP_SAD *sa_lcp = new LCP_SAD(csa, lcp_verb, text, n);
		// for (uint i=0;i<n;i++){
		// 	assert(sa_lcp->get_LCP(i, csa)==lcp_verb[i]);
		// }
		uint *O_aux, *Z_aux;
		b_length = sa_lcp->U_length;
		generate_OZ(sa_lcp->U, &O_aux, &Z_aux, b_length);

		lcp_count=0;

		if(op_rs==DELTA){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);
			uint *Obit=new uint[b_length];
			for(int i=0; i<b_length; ++i)
				Obit[i]=0;

			uint aux2, l1,l2;
			uint aux=O->select1(1);
			Obit[0]= aux+1;
			for(l1=2; l1<b_length; ++l1){
				aux2=O->select1(l1);
				if(aux2-aux){//>0
					Obit[l1-1]= aux2-aux;
					aux=aux2;
				}
				else break;
			}
			cout << "l1:" <<l1 << endl;

			aux=Z->select1(1);
			Obit[0]= aux+1;
			for(l2=2; l2<b_length; ++l2){
				aux2=Z->select1(l2);
				if(aux2-aux){
					Obit[l2-1]= aux2-aux;
					aux=aux2;
				}
				else break;
			}
			cout << "l2:" <<l2 << endl;
			Z = new BitSequenceDelta(Obit, l2);

		}
		else if(op_rs==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);

		}
		else{
			O = new BitSequenceSDArray(O_aux, b_length);
			Z = new BitSequenceSDArray(Z_aux, b_length);
		}

		delete (LCP_SAD *)sa_lcp;
		delete [] O_aux;
		delete [] Z_aux;

		//for(int i=0; i<b_length; ++i)
		//  cout <<"i: "<< i << " O[i]: " << O->select1(i) << endl;

	}
/*
	LCP_FMN_RLCSA::LCP_FMN_RLCSA(LCP *lcp, TextIndex *csa, size_t n, size_t op_rs){
		uint *O_aux, *Z_aux;
		if(op_rs!=RRR02_HDR && op_rs!=SDARRAY_HDR){
			cout << "Error: op_rs must be RRR02_HDR or SDARRAY_HDR\n" << endl;
			exit(1);
		}
		lcp_type = FMN_RRR_OS;
		OZ_type = op_rs;
		LCP_SAD *sa_lcp = new LCP_SAD(lcp, csa, n);
		b_length = sa_lcp->U_length;
		generate_OZ(sa_lcp->U, &O_aux, &Z_aux, b_length);
		if(op_rs==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);
		}
		else{
			O = new BitSequenceSDArray(O_aux, b_length);
			Z = new BitSequenceSDArray(Z_aux, b_length);
		}
		delete (LCP_SAD *)sa_lcp;
		delete [] O_aux;
		delete [] Z_aux;
	}
*/
    int LCP_FMN_RLCSA::lcp_access()const{
     return lcp_count;
   }
	size_t LCP_FMN_RLCSA::get_LCP(size_t i, TextIndex *csa) const{ 
	        ++lcp_count;
		size_t val = csa->getSA(i);
		size_t rank_0 = O->rank1(val);
                //  cout << "i: "<<i<< " access: " << O->access(i) << endl;
		if(rank_0 > 0){
			size_t r =   Z->select1(rank_0) - val +1;
			return r;
		}
		else
			return 0;
	}


	size_t LCP_FMN_RLCSA::get_LCP(size_t i, size_t sa_i) const{
		++lcp_count;
		size_t val = sa_i;
		size_t rank_0 = O->rank1(val);
		//  cout << "i: "<<i<< " access: " << O->access(i) << endl;
		if(rank_0 > 0){
			size_t r =   Z->select1(rank_0) - val +1;
			return r;
		}
		else
			return 0;
	}

	size_t LCP_FMN_RLCSA::get_seq_LCP(size_t i, TextIndex *csa, size_t **next_pos, size_t *n_next, bool dir) const{
		return get_LCP(i,csa);
	}

	size_t LCP_FMN_RLCSA::getSize() const{
        return O->getSize() + Z->getSize() + sizeof(LCP_FMN_RLCSA);
	}

	void LCP_FMN_RLCSA::save(ofstream & fp) const{
		saveValue(fp,lcp_type);
		saveValue(fp,b_length);
		saveValue(fp,OZ_type);
		O->save(fp);
		Z->save(fp);
	}

	LCP_FMN_RLCSA* LCP_FMN_RLCSA::load(ifstream & fp){
		LCP_FMN_RLCSA *lcp = new LCP_FMN_RLCSA();
		size_t type = loadValue<size_t>(fp);
		if(type!=FMN_RRR_OS) {
			abort();
		}
		lcp->b_length = loadValue<size_t>(fp);
		lcp->OZ_type = loadValue<size_t>(fp);
                lcp->O = BitSequence::load(fp);
                lcp->Z = BitSequence::load(fp);
              

		return lcp;
	}

	LCP_FMN_RLCSA::~LCP_FMN_RLCSA(){
		if(O!= NULL){
			if(OZ_type == RRR02_HDR)
				delete (BitSequenceRRR *)O;
			else if(OZ_type == SDARRAY_HDR)
				delete (BitSequenceSDArray *)O;
                        else
                            delete (BitSequenceDelta *)O;
		}
		if(Z!=NULL){
			if(OZ_type == RRR02_HDR)
				delete (BitSequenceRRR *)Z;
			else if(OZ_type == SDARRAY_HDR)
				delete (BitSequenceSDArray *)Z;
                        else
                            delete (BitSequenceDelta *)Z;
		}
	}

};
