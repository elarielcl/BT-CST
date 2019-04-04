/********************************************************************************
Copyright (c) 2015, Alberto Ordóñez.
All rights reserved.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

********************************************************************************/

#include <CST.h>

using namespace cds_static;

int main(int argc, char **argv){
    if (argc<2){
        cerr << "Usage: " << argv[0] << " <input-text> <base-name-output> " << endl;
        return 0;
    }
    char * input_text = argv[1];
    char * base_name_output = argv[2];
    uint sampling_rules = 0;
    uint sampling_C = 10;
    typeSelfIndex type = trlcsa;
    CST *cst = new CST(input_text, base_name_output, trlcsa, sampling_C, sampling_rules);

    //cerr << "CST size: " << cst->getSize() << endl;

    std::cout << cst->fChild(cst->root()) << std::endl;
    return 0;
}