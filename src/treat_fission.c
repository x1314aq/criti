//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "particle_state.h"
#include "map.h"
#include "nuclide.h"
#include "acedata.h"
#include "criticality.h"

extern particle_state_t base_par_state;
extern map *base_nucs;
extern criti_t base_criti;

static int fis_MT[5] = {18, 19, 20, 21, 38};

void treat_fission(){
    nuclide_t *nuc;
    double fis_sub_cs[5];

    nuc = (nuclide_t *) map_get(base_nucs, base_par_state.nuc);

    if(nuc->fis <= ZERO) return;

    if(nuc->LSIG[18] > 0)  /* 总裂变截面MT=18存在 */
        fis_sub_cs[0] = get_nuc_mt_cs(nuc, 18, base_par_state.interp_N0, base_par_state.interp_K0);
    else
        for(int i = 1; i < 5; i++)
            fis_sub_cs[i] = get_nuc_mt_cs(nuc, fis_MT[i], base_par_state.interp_N0, base_par_state.interp_K0);

    Estimate_keff_abs(base_par_state.wgt, nuc->nu, nuc->fis, nuc->tot);

    double fis_R;
    if(nuc->LSIG[18] > 0){
        if(fis_sub_cs[0] > 0){
            fis_R = base_par_state.wgt * nuc->nu * fis_sub_cs[0] / nuc->tot / base_criti.keff_final;
            get_fis_neu_state(fis_MT[0], fis_R);
        }
    } else{
        for(int i = 1; i < 5; i++){
            if(fis_sub_cs[i] > 0){
                fis_R = base_par_state.wgt * nuc->nu * fis_sub_cs[i] / nuc->tot / base_criti.keff_final;
                get_fis_neu_state(fis_MT[i], fis_R);
            }
        }
    }
}