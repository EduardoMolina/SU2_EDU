/*!
 * \file numerics_structure.cpp
 * \brief This file contains all the numerical methods.
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 1.0.0
 *
 * SU2, Copyright (C) 2012-2014 Aerospace Design Laboratory (ADL).
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/numerics_structure.hpp"

CNumerics::CNumerics(void) { }

CNumerics::CNumerics(unsigned short val_nDim, unsigned short val_nVar,
                     CConfig *config) {
	nDim = val_nDim;
	nVar = val_nVar;
	Gamma = config->GetGamma();
	Gamma_Minus_One = Gamma - 1.0;
	Gas_Constant = config->GetGas_ConstantND();
  
	//U_id = new double [nVar];
	//U_jd = new double [nVar];
  
	UnitNormal = new double [nDim];
	UnitNormald = new double [nDim];
  
	Normal = new double [nDim];
	Flux_Tensor = new double* [nVar];
	for (unsigned short iVar = 0; iVar < (nVar); iVar++)
  Flux_Tensor[iVar] = new double [nDim];
  
	tau = new double* [nDim];
	delta = new double* [nDim];
	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		tau[iDim] = new double [nDim];
		delta[iDim] = new double [nDim];
	}
  
	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		for (unsigned short jDim = 0; jDim < nDim; jDim++) {
			if (iDim==jDim) delta[iDim][jDim]=1.0;
			else delta[iDim][jDim]=0.0;
		}
	}
  
	U_n = new double [nVar];
	U_nM1 = new double [nVar];
	U_nP1 = new double [nVar];
  
	Proj_Flux_Tensor = new double [nVar];
  
	turb_ke_i = 0.0;
	turb_ke_j = 0.0;
  
  Diffusion_Coeff_i = NULL;
  Diffusion_Coeff_j = NULL;
  
  Vector = new double[nDim];
  
  l = new double [nDim];
  m = new double [nDim];
}

CNumerics::~CNumerics(void) {
  
  delete [] Normal;
	delete [] UnitNormal;
  
	delete [] U_n;
	delete [] U_nM1;
	delete [] U_nP1;
  
	// visc
	delete [] Proj_Flux_Tensor;
  
	for (unsigned short iVar = 0; iVar < nDim+3; iVar++) {
		delete [] Flux_Tensor[iVar];
	}
	delete [] Flux_Tensor;
  
	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		delete [] tau[iDim];
		delete [] delta[iDim];
	}
	delete [] tau;
	delete [] delta;
	delete [] Normal;
	delete [] Theta_v;
  if (Ys != NULL) delete [] Ys;
  if (dFdYi != NULL) {
    delete [] dFdYi;
  }
  if (dFdYj != NULL) {
    delete [] dFdYj;
  }
  if (sumdFdYih != NULL) delete [] sumdFdYih;
  if (sumdFdYjh != NULL) delete [] sumdFdYjh;
  if (sumdFdYieve != NULL) delete [] sumdFdYieve;
  if (sumdFdYjeve != NULL) delete [] sumdFdYjeve;
  if (Diffusion_Coeff_i != NULL) delete [] Diffusion_Coeff_i;
  if (Diffusion_Coeff_j != NULL) delete [] Diffusion_Coeff_j;
  if (Vector != NULL) delete [] Vector;
  
	unsigned short iVar;
	for (iVar = 0; iVar < nVar; iVar++) {
		delete [] dVdU[iVar];
	}
	delete [] dVdU;
  
}

void CNumerics::GetInviscidFlux(double val_density, double *val_velocity,
                                double val_pressure, double val_enthalpy) {
	if (nDim == 3) {
		Flux_Tensor[0][0] = val_density*val_velocity[0];
		Flux_Tensor[1][0] = Flux_Tensor[0][0]*val_velocity[0]+val_pressure;
		Flux_Tensor[2][0] = Flux_Tensor[0][0]*val_velocity[1];
		Flux_Tensor[3][0] = Flux_Tensor[0][0]*val_velocity[2];
		Flux_Tensor[4][0] = Flux_Tensor[0][0]*val_enthalpy;
    
		Flux_Tensor[0][1] = val_density*val_velocity[1];
		Flux_Tensor[1][1] = Flux_Tensor[0][1]*val_velocity[0];
		Flux_Tensor[2][1] = Flux_Tensor[0][1]*val_velocity[1]+val_pressure;
		Flux_Tensor[3][1] = Flux_Tensor[0][1]*val_velocity[2];
		Flux_Tensor[4][1] = Flux_Tensor[0][1]*val_enthalpy;
    
		Flux_Tensor[0][2] = val_density*val_velocity[2];
		Flux_Tensor[1][2] = Flux_Tensor[0][2]*val_velocity[0];
		Flux_Tensor[2][2] = Flux_Tensor[0][2]*val_velocity[1];
		Flux_Tensor[3][2] = Flux_Tensor[0][2]*val_velocity[2]+val_pressure;
		Flux_Tensor[4][2] = Flux_Tensor[0][2]*val_enthalpy;
    
	}
	if(nDim == 2) {
		Flux_Tensor[0][0] = val_density*val_velocity[0];
		Flux_Tensor[1][0] = Flux_Tensor[0][0]*val_velocity[0]+val_pressure;
		Flux_Tensor[2][0] = Flux_Tensor[0][0]*val_velocity[1];
		Flux_Tensor[3][0] = Flux_Tensor[0][0]*val_enthalpy;
    
		Flux_Tensor[0][1] = val_density*val_velocity[1];
		Flux_Tensor[1][1] = Flux_Tensor[0][1]*val_velocity[0];
		Flux_Tensor[2][1] = Flux_Tensor[0][1]*val_velocity[1]+val_pressure;
		Flux_Tensor[3][1] = Flux_Tensor[0][1]*val_enthalpy;
	}
}

void CNumerics::GetInviscidProjFlux(double *val_density,
                                    double *val_velocity,
                                    double *val_pressure,
                                    double *val_enthalpy,
                                    double *val_normal,
                                    double *val_Proj_Flux) {
  double rhou, rhov, rhow;
  
  
	if (nDim == 2) {
		rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
    
		val_Proj_Flux[0] = rhou*val_normal[0];
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0];
		val_Proj_Flux[2] = rhou*val_velocity[1]*val_normal[0];
		val_Proj_Flux[3] = rhou*(*val_enthalpy)*val_normal[0];
    
		val_Proj_Flux[0] += rhov*val_normal[1];
		val_Proj_Flux[1] += rhov*val_velocity[0]*val_normal[1];
		val_Proj_Flux[2] += (rhov*val_velocity[1]+(*val_pressure))*val_normal[1];
		val_Proj_Flux[3] += rhov*(*val_enthalpy)*val_normal[1];
	}
	else {
		rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
		rhow = (*val_density)*val_velocity[2];
    
		val_Proj_Flux[0] = rhou*val_normal[0];
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0];
		val_Proj_Flux[2] = rhou*val_velocity[1]*val_normal[0];
		val_Proj_Flux[3] = rhou*val_velocity[2]*val_normal[0];
		val_Proj_Flux[4] = rhou*(*val_enthalpy)*val_normal[0];
    
		val_Proj_Flux[0] += rhov*val_normal[1];
		val_Proj_Flux[1] += rhov*val_velocity[0]*val_normal[1];
		val_Proj_Flux[2] += (rhov*val_velocity[1]+(*val_pressure))*val_normal[1];
		val_Proj_Flux[3] += rhov*val_velocity[2]*val_normal[1];
		val_Proj_Flux[4] += rhov*(*val_enthalpy)*val_normal[1];
    
		val_Proj_Flux[0] += rhow*val_normal[2];
		val_Proj_Flux[1] += rhow*val_velocity[0]*val_normal[2];
		val_Proj_Flux[2] += rhow*val_velocity[1]*val_normal[2];
		val_Proj_Flux[3] += (rhow*val_velocity[2]+(*val_pressure))*val_normal[2];
		val_Proj_Flux[4] += rhow*(*val_enthalpy)*val_normal[2];
	}
  
}

void CNumerics::GetInviscidProjJac(double *val_velocity, double *val_energy,
                                   double *val_normal, double val_scale,
                                   double **val_Proj_Jac_Tensor) {
	unsigned short iDim, jDim;
  double sqvel, proj_vel, phi, a1, a2;
  
	sqvel = 0.0, proj_vel = 0.0;
	for (iDim = 0; iDim < nDim; iDim++) {
		sqvel    += val_velocity[iDim]*val_velocity[iDim];
		proj_vel += val_velocity[iDim]*val_normal[iDim];
	}
  
	phi = 0.5*Gamma_Minus_One*sqvel;
	a1 = Gamma*(*val_energy)-phi;
	a2 = Gamma-1.0;
  
	val_Proj_Jac_Tensor[0][0] = 0.0;
	for (iDim = 0; iDim < nDim; iDim++)
  val_Proj_Jac_Tensor[0][iDim+1] = val_scale*val_normal[iDim];
	val_Proj_Jac_Tensor[0][nDim+1] = 0.0;
  
	for (iDim = 0; iDim < nDim; iDim++) {
		val_Proj_Jac_Tensor[iDim+1][0] = val_scale*(val_normal[iDim]*phi - val_velocity[iDim]*proj_vel);
		for (jDim = 0; jDim < nDim; jDim++)
    val_Proj_Jac_Tensor[iDim+1][jDim+1] = val_scale*(val_normal[jDim]*val_velocity[iDim]-a2*val_normal[iDim]*val_velocity[jDim]);
		val_Proj_Jac_Tensor[iDim+1][iDim+1] += val_scale*proj_vel;
		val_Proj_Jac_Tensor[iDim+1][nDim+1] = val_scale*a2*val_normal[iDim];
	}
  
	val_Proj_Jac_Tensor[nDim+1][0] = val_scale*proj_vel*(phi-a1);
	for (iDim = 0; iDim < nDim; iDim++)
  val_Proj_Jac_Tensor[nDim+1][iDim+1] = val_scale*(val_normal[iDim]*a1-a2*val_velocity[iDim]*proj_vel);
	val_Proj_Jac_Tensor[nDim+1][nDim+1] = val_scale*Gamma*proj_vel;
}

void CNumerics::SetPastSol (double *val_u_nM1, double *val_u_n, double *val_u_nP1) {
	unsigned short iVar;
  
	for(iVar = 0; iVar < nVar; iVar++) {
		U_nM1[iVar] = val_u_nM1[iVar];
		U_n[iVar] = val_u_n[iVar];
		U_nP1[iVar] = val_u_nP1[iVar];
	}
  
}
void CNumerics::SetPastVolume (double val_volume_nM1, double val_volume_n, double val_volume_nP1) {
	Volume_nM1 = val_volume_nM1;
	Volume_n = val_volume_n;
	Volume_nP1 = val_volume_nP1;
}


void CNumerics::GetPMatrix(double *val_density, double *val_velocity,
                           double *val_soundspeed, double *val_normal, double **val_p_tensor) {
  //************************************************//
  // Please do not delete //SU2_CPP2C comment lines //
  //************************************************//
  
  //SU2_CPP2C SUB START GetPMatrix
  //SU2_CPP2C SUB VARS *val_density val_velocity *val_soundspeed val_p_tensor val_normal
  
	double sqvel, rhooc, rhoxc, c2;
  
	rhooc = *val_density / *val_soundspeed,
  rhoxc = *val_density * *val_soundspeed,
  c2 = *val_soundspeed * *val_soundspeed;
  
	if(nDim == 2) {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];
    
		val_p_tensor[0][0]=1.0;
		val_p_tensor[0][1]=0.0;
		val_p_tensor[0][2]=0.5*rhooc;
		val_p_tensor[0][3]=0.5*rhooc;
    
		val_p_tensor[1][0]=val_velocity[0];
		val_p_tensor[1][1]=*val_density*val_normal[1];
		val_p_tensor[1][2]=0.5*(val_velocity[0]*rhooc+val_normal[0]**val_density);
		val_p_tensor[1][3]=0.5*(val_velocity[0]*rhooc-val_normal[0]**val_density);
    
		val_p_tensor[2][0]=val_velocity[1];
		val_p_tensor[2][1]=-*val_density*val_normal[0];
		val_p_tensor[2][2]=0.5*(val_velocity[1]*rhooc+val_normal[1]**val_density);
		val_p_tensor[2][3]=0.5*(val_velocity[1]*rhooc-val_normal[1]**val_density);
    
		val_p_tensor[3][0]=0.5*sqvel;
		val_p_tensor[3][1]=*val_density*val_velocity[0]*val_normal[1]-*val_density*val_velocity[1]*val_normal[0];
		val_p_tensor[3][2]=0.5*(0.5*sqvel*rhooc+*val_density*val_velocity[0]*val_normal[0]+*val_density*val_velocity[1]*val_normal[1]+rhoxc/Gamma_Minus_One);
		val_p_tensor[3][3]=0.5*(0.5*sqvel*rhooc-*val_density*val_velocity[0]*val_normal[0]-*val_density*val_velocity[1]*val_normal[1]+rhoxc/Gamma_Minus_One);
	}
	else {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];
    
		val_p_tensor[0][0]=val_normal[0];
		val_p_tensor[0][1]=val_normal[1];
		val_p_tensor[0][2]=val_normal[2];
		val_p_tensor[0][3]=0.5*rhooc;
		val_p_tensor[0][4]=0.5*rhooc;
    
		val_p_tensor[1][0]=val_velocity[0]*val_normal[0];
		val_p_tensor[1][1]=val_velocity[0]*val_normal[1]-*val_density*val_normal[2];
		val_p_tensor[1][2]=val_velocity[0]*val_normal[2]+*val_density*val_normal[1];
		val_p_tensor[1][3]=0.5*(val_velocity[0]*rhooc+*val_density*val_normal[0]);
		val_p_tensor[1][4]=0.5*(val_velocity[0]*rhooc-*val_density*val_normal[0]);
    
		val_p_tensor[2][0]=val_velocity[1]*val_normal[0]+*val_density*val_normal[2];
		val_p_tensor[2][1]=val_velocity[1]*val_normal[1];
		val_p_tensor[2][2]=val_velocity[1]*val_normal[2]-*val_density*val_normal[0];
		val_p_tensor[2][3]=0.5*(val_velocity[1]*rhooc+*val_density*val_normal[1]);
		val_p_tensor[2][4]=0.5*(val_velocity[1]*rhooc-*val_density*val_normal[1]);
    
		val_p_tensor[3][0]=val_velocity[2]*val_normal[0]-*val_density*val_normal[1];
		val_p_tensor[3][1]=val_velocity[2]*val_normal[1]+*val_density*val_normal[0];
		val_p_tensor[3][2]=val_velocity[2]*val_normal[2];
		val_p_tensor[3][3]=0.5*(val_velocity[2]*rhooc+*val_density*val_normal[2]);
		val_p_tensor[3][4]=0.5*(val_velocity[2]*rhooc-*val_density*val_normal[2]);
    
		val_p_tensor[4][0]=0.5*sqvel*val_normal[0]+*val_density*val_velocity[1]*val_normal[2]-*val_density*val_velocity[2]*val_normal[1];
		val_p_tensor[4][1]=0.5*sqvel*val_normal[1]-*val_density*val_velocity[0]*val_normal[2]+*val_density*val_velocity[2]*val_normal[0];
		val_p_tensor[4][2]=0.5*sqvel*val_normal[2]+*val_density*val_velocity[0]*val_normal[1]-*val_density*val_velocity[1]*val_normal[0];
		val_p_tensor[4][3]=0.5*(0.5*sqvel*rhooc+*val_density*(val_velocity[0]*val_normal[0]+val_velocity[1]*val_normal[1]+val_velocity[2]*val_normal[2])+rhoxc/Gamma_Minus_One);
		val_p_tensor[4][4]=0.5*(0.5*sqvel*rhooc-*val_density*(val_velocity[0]*val_normal[0]+val_velocity[1]*val_normal[1]+val_velocity[2]*val_normal[2])+rhoxc/Gamma_Minus_One);
	}
  
  //SU2_CPP2C SUB END GetPMatrix
}

void CNumerics::GetPMatrix_inv(double *val_density, double *val_velocity,
                               double *val_soundspeed, double *val_normal, double **val_invp_tensor) {
	double rhoxc, c2, gm1, k0orho, k1orho, gm1_o_c2, gm1_o_rhoxc, sqvel;
  
	rhoxc = *val_density * *val_soundspeed;
	c2 = *val_soundspeed * *val_soundspeed;
	gm1 = Gamma_Minus_One;
	k0orho = val_normal[0] / *val_density;
	k1orho = val_normal[1] / *val_density;
	gm1_o_c2 = gm1/c2;
	gm1_o_rhoxc = gm1/rhoxc;
  
	if (nDim == 3) {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];
    
		val_invp_tensor[0][0]=val_normal[0]-val_normal[2]*val_velocity[1] / *val_density+val_normal[1]*val_velocity[2] / *val_density-val_normal[0]*0.5*gm1*sqvel/c2;
		val_invp_tensor[0][1]=val_normal[0]*gm1*val_velocity[0]/c2;
		val_invp_tensor[0][2]=val_normal[2] / *val_density+val_normal[0]*gm1*val_velocity[1]/c2;
		val_invp_tensor[0][3]=-val_normal[1] / *val_density+val_normal[0]*gm1*val_velocity[2]/c2;
		val_invp_tensor[0][4]=-val_normal[0]*gm1/c2;
    
		val_invp_tensor[1][0]=val_normal[1]+val_normal[2]*val_velocity[0] / *val_density-val_normal[0]*val_velocity[2] / *val_density-val_normal[1]*0.5*gm1*sqvel/c2;
		val_invp_tensor[1][1]=-val_normal[2] / *val_density+val_normal[1]*gm1*val_velocity[0]/c2;
		val_invp_tensor[1][2]=val_normal[1]*gm1*val_velocity[1]/c2;
		val_invp_tensor[1][3]=val_normal[0] / *val_density+val_normal[1]*gm1*val_velocity[2]/c2;
		val_invp_tensor[1][4]=-val_normal[1]*gm1/c2;
    
		val_invp_tensor[2][0]=val_normal[2]-val_normal[1]*val_velocity[0] / *val_density+val_normal[0]*val_velocity[1] / *val_density-val_normal[2]*0.5*gm1*sqvel/c2;
		val_invp_tensor[2][1]=val_normal[1] / *val_density+val_normal[2]*gm1*val_velocity[0]/c2;
		val_invp_tensor[2][2]=-val_normal[0] / *val_density+val_normal[2]*gm1*val_velocity[1]/c2;
		val_invp_tensor[2][3]=val_normal[2]*gm1*val_velocity[2]/c2;
		val_invp_tensor[2][4]=-val_normal[2]*gm1/c2;
    
		val_invp_tensor[3][0]=-(val_normal[0]*val_velocity[0]+val_normal[1]*val_velocity[1]+val_normal[2]*val_velocity[2]) / *val_density+0.5*gm1*sqvel/rhoxc;
		val_invp_tensor[3][1]=val_normal[0] / *val_density-gm1*val_velocity[0]/rhoxc;
		val_invp_tensor[3][2]=val_normal[1] / *val_density-gm1*val_velocity[1]/rhoxc;
		val_invp_tensor[3][3]=val_normal[2] / *val_density-gm1*val_velocity[2]/rhoxc;
		val_invp_tensor[3][4]=Gamma_Minus_One/rhoxc;
    
		val_invp_tensor[4][0]=(val_normal[0]*val_velocity[0]+val_normal[1]*val_velocity[1]+val_normal[2]*val_velocity[2]) / *val_density+0.5*gm1*sqvel/rhoxc;
		val_invp_tensor[4][1]=-val_normal[0] / *val_density-gm1*val_velocity[0]/rhoxc;
		val_invp_tensor[4][2]=-val_normal[1] / *val_density-gm1*val_velocity[1]/rhoxc;
		val_invp_tensor[4][3]=-val_normal[2] / *val_density-gm1*val_velocity[2]/rhoxc;
		val_invp_tensor[4][4]=Gamma_Minus_One/rhoxc;
	}
	if(nDim == 2) {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];
    
		val_invp_tensor[0][0]=1.0-0.5*gm1_o_c2*sqvel;
		val_invp_tensor[0][1]=gm1_o_c2*val_velocity[0];
		val_invp_tensor[0][2]=gm1_o_c2*val_velocity[1];
		val_invp_tensor[0][3]=-gm1_o_c2;
    
		val_invp_tensor[1][0]=-k1orho*val_velocity[0]+k0orho*val_velocity[1];
		val_invp_tensor[1][1]=k1orho;
		val_invp_tensor[1][2]=-k0orho;
		val_invp_tensor[1][3]=0.0;
    
		val_invp_tensor[2][0]=-k0orho*val_velocity[0]-k1orho*val_velocity[1]+0.5*gm1_o_rhoxc*sqvel;
		val_invp_tensor[2][1]=k0orho-gm1_o_rhoxc*val_velocity[0];
		val_invp_tensor[2][2]=k1orho-gm1_o_rhoxc*val_velocity[1];
		val_invp_tensor[2][3]=gm1_o_rhoxc;
    
		val_invp_tensor[3][0]=k0orho*val_velocity[0]+k1orho*val_velocity[1]+0.5*gm1_o_rhoxc*sqvel;
		val_invp_tensor[3][1]=-k0orho-gm1_o_rhoxc*val_velocity[0];
		val_invp_tensor[3][2]=-k1orho-gm1_o_rhoxc*val_velocity[1];
		val_invp_tensor[3][3]=gm1_o_rhoxc;
	}
}

void CNumerics::GetinvRinvPe(double Beta2, double val_enthalpy,
                             double val_soundspeed, double val_density,
                             double* val_velocity, double **invRinvPe) {
  
	double sqvel;
	double factor = 1.0/(val_soundspeed*val_soundspeed*Beta2);
  
	if(nDim == 2) {
    
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];
    
		invRinvPe[0][0] = factor;
		invRinvPe[0][1] = 0.0;
		invRinvPe[0][2] = 0.0;
		invRinvPe[0][3] = -val_density/Gamma;
    
		invRinvPe[1][0] = val_velocity[0]*factor;
		invRinvPe[1][1] = val_density;
		invRinvPe[1][2] = 0.0;
		invRinvPe[1][3] = -val_density*val_velocity[0]/Gamma;
    
		invRinvPe[2][0] = val_velocity[1]*factor;
		invRinvPe[2][1] = 0;
		invRinvPe[2][2] = val_density;
		invRinvPe[2][3] = -val_density*val_velocity[1]/Gamma;
    
		invRinvPe[3][0] = val_enthalpy*factor;
		invRinvPe[3][1] = val_density*val_velocity[0];
		invRinvPe[3][2] = val_density*val_velocity[1];
		invRinvPe[3][3] = -val_density*sqvel/(2.0*Gamma);
	}
	else {
    
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];
    
		invRinvPe[0][0] =  factor;
		invRinvPe[0][1] = 0.0;
		invRinvPe[0][2] = 0.0;
		invRinvPe[0][3] = 0.0;
		invRinvPe[0][4] = -val_density/Gamma;
    
		invRinvPe[1][0] = val_velocity[0]*factor;
		invRinvPe[1][1] = val_density;
		invRinvPe[1][2] = 0.0;
		invRinvPe[1][3] = 0.0;
		invRinvPe[1][4] = -val_density*val_velocity[0]/Gamma;
    
		invRinvPe[2][0] = val_velocity[1]*factor;
		invRinvPe[2][1] = 0;
		invRinvPe[2][2] = val_density;
		invRinvPe[2][3] = 0.0;
		invRinvPe[2][4] = -val_density*val_velocity[1]/Gamma;
    
    
		invRinvPe[3][0] = val_velocity[2]*factor;
		invRinvPe[3][1] = 0;
		invRinvPe[3][2] = 0;
		invRinvPe[3][3] = val_density;
		invRinvPe[3][4] = -val_density*val_velocity[2]/Gamma;
    
		invRinvPe[4][0] = val_enthalpy*factor;
		invRinvPe[4][1] = val_density*val_velocity[0];
		invRinvPe[4][2] = val_density*val_velocity[1];
		invRinvPe[4][3] = val_density*val_velocity[2];
		invRinvPe[4][4] = -val_density*sqvel/(2.0*Gamma);
    
	}
  
}

void CNumerics::GetRMatrix(double val_pressure, double val_soundspeed, double val_density, double* val_velocity, double **R_Matrix) {
  
	double sqvel;
	//double factor = 1.0/(val_soundspeed*val_soundspeed*1);
	double gm1 = Gamma - 1.0;
  
	if(nDim == 2) {
    
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];
    
		R_Matrix[0][0] =  0.5*gm1*sqvel;
		R_Matrix[0][1] = -val_velocity[0]*gm1;
		R_Matrix[0][2] = -val_velocity[1]*gm1;
		R_Matrix[0][3] = gm1;
    
		R_Matrix[1][0] = -val_velocity[0]/val_density;
		R_Matrix[1][1] = 1.0/val_density;
		R_Matrix[1][2] = 0.0;
		R_Matrix[1][3] = 0.0;
    
		R_Matrix[2][0] = -val_velocity[1]/val_density;
		R_Matrix[2][1] = 0.0;
		R_Matrix[2][2] = 1.0/val_density;
		R_Matrix[2][3] = 0.0;
    
		R_Matrix[3][0] = 0.5*gm1*sqvel/val_pressure - Gamma/val_density;
		R_Matrix[3][1] = -gm1*val_velocity[0]/val_pressure;
		R_Matrix[3][2] = -gm1*val_velocity[1]/val_pressure;
		R_Matrix[3][3] = gm1/val_pressure;
	}
	else {
    
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];
    
		R_Matrix[0][0] =  0.5*gm1*sqvel;
		R_Matrix[0][1] = -val_velocity[0]*gm1;
		R_Matrix[0][2] = -val_velocity[1]*gm1;
		R_Matrix[0][3] = -val_velocity[2]*gm1;
		R_Matrix[0][4] = gm1;
    
		R_Matrix[1][0] = -val_velocity[0]/val_density;
		R_Matrix[1][1] = 1.0/val_density;
		R_Matrix[1][2] = 0.0;
		R_Matrix[1][3] = 0.0;
		R_Matrix[1][4] = 0.0;
    
		R_Matrix[2][0] = -val_velocity[1]/val_density;
		R_Matrix[2][1] = 0.0;
		R_Matrix[2][2] = 1.0/val_density;
		R_Matrix[2][3] = 0.0;
		R_Matrix[2][4] = 0.0;
    
		R_Matrix[3][0] = -val_velocity[2]/val_density;
		R_Matrix[3][1] = 0.0;
		R_Matrix[3][2] = 0.0;
		R_Matrix[3][3] = 1.0/val_density;
		R_Matrix[3][4] = 0.0;
    
		R_Matrix[4][0] = 0.5*gm1*sqvel/val_pressure - Gamma/val_density;
		R_Matrix[4][1] = -gm1*val_velocity[0]/val_pressure;
		R_Matrix[4][2] = -gm1*val_velocity[1]/val_pressure;
		R_Matrix[4][3] = -gm1*val_velocity[2]/val_pressure;
		R_Matrix[4][4] = gm1/val_pressure;
    
	}
  
}


void CNumerics::GetPrecondJacobian(double Beta2, double r_hat, double s_hat, double t_hat, double rB2a2, double* Lambda, double *val_normal,
                                   double **val_absPeJac) {
  
	double lam1, lam2, lam3, lam4;
	lam1 = Lambda[0]; lam2 = Lambda[1]; lam3 = Lambda[2]; lam4 = Lambda[3];
  
	if(nDim == 2) {
    
		val_absPeJac[0][0] =  lam3*s_hat/(2.0*t_hat) - lam4*r_hat/(2.0*t_hat);
		val_absPeJac[0][1] = -lam3*rB2a2*val_normal[0]/(2.0*t_hat) + lam4*rB2a2*val_normal[0]/(2.0*t_hat);
		val_absPeJac[0][2] = -lam3*rB2a2*val_normal[1]/(2.0*t_hat) + lam4*rB2a2*val_normal[1]/(2.0*t_hat);
		val_absPeJac[0][3] =  0.0;
    
		val_absPeJac[1][0] = r_hat*val_normal[0]*lam3*s_hat/(2.0*t_hat*rB2a2) + s_hat*val_normal[0]*lam4*(-r_hat)/(2.0*t_hat*rB2a2);
		val_absPeJac[1][1] = lam2*(val_normal[1]*val_normal[1]) - lam3*r_hat*val_normal[0]*val_normal[0]/(2.0*t_hat) + lam4*s_hat*val_normal[0]*val_normal[0]/(2.0*t_hat);
		val_absPeJac[1][2] = -lam2*val_normal[0]*val_normal[1] - lam3*r_hat*val_normal[0]*val_normal[1]/(2.0*t_hat) + lam4*s_hat*val_normal[0]*val_normal[1]/(2.0*t_hat);
		val_absPeJac[1][3] = 0.0;
    
		val_absPeJac[2][0] = lam3*r_hat*val_normal[1]*s_hat/(2.0*t_hat*rB2a2) - s_hat*val_normal[1]*lam4*r_hat/(2.0*t_hat*rB2a2);
		val_absPeJac[2][1] = -val_normal[0]*val_normal[1]*lam2 - r_hat*val_normal[1]*val_normal[0]*lam3/(2.0*t_hat) + s_hat*val_normal[0]*val_normal[1]*lam4/(2.0*t_hat);
		val_absPeJac[2][2] = val_normal[0]*val_normal[0]*lam2 -r_hat*val_normal[1]*val_normal[1]*lam3/(2.0*t_hat) + s_hat*val_normal[1]*val_normal[1]*lam4/(2.0*t_hat);
		val_absPeJac[2][3] = 0.0;
    
		val_absPeJac[3][0] = 0.0;
		val_absPeJac[3][1] = 0.0;
		val_absPeJac[3][2] = 0.0;
		val_absPeJac[3][3] = lam1;
    
	}
	else {
    
		double lam5 = Lambda[4];
    
		val_absPeJac[0][0] =  lam4*s_hat/(2.0*t_hat) - lam5*r_hat/(2.0*t_hat);
		val_absPeJac[0][1] = -lam4*rB2a2*val_normal[0]/(2.0*t_hat) + lam5*rB2a2*val_normal[0]/(2.0*t_hat);
		val_absPeJac[0][2] = -lam4*rB2a2*val_normal[1]/(2.0*t_hat) + lam5*rB2a2*val_normal[1]/(2.0*t_hat);
		val_absPeJac[0][3] = -lam4*rB2a2*val_normal[2]/(2.0*t_hat) + lam5*rB2a2*val_normal[2]/(2.0*t_hat);
		val_absPeJac[0][4] =  0.0;
    
		val_absPeJac[1][0] = r_hat*val_normal[0]*lam4*s_hat/(2.0*t_hat*rB2a2) + s_hat*val_normal[0]*lam5*(-r_hat)/(2.0*t_hat*rB2a2);
		val_absPeJac[1][1] = lam2*(val_normal[2]*val_normal[2] + val_normal[1]*val_normal[1]) - lam4*r_hat*val_normal[0]*val_normal[0]/(2.0*t_hat) + lam5*s_hat*val_normal[0]*val_normal[0]/(2.0*t_hat);
		val_absPeJac[1][2] = -lam2*val_normal[0]*val_normal[1] - lam4*r_hat*val_normal[0]*val_normal[1]/(2.0*t_hat) + lam5*s_hat*val_normal[0]*val_normal[1]/(2.0*t_hat);
		val_absPeJac[1][3] = -lam2*val_normal[0]*val_normal[2] - lam4*r_hat*val_normal[0]*val_normal[2]/(2.0*t_hat) + lam5*s_hat*val_normal[0]*val_normal[2]/(2.0*t_hat);
		val_absPeJac[1][4] = 0.0;
    
		val_absPeJac[2][0] = lam4*r_hat*val_normal[1]*s_hat/(2.0*t_hat*rB2a2) - s_hat*val_normal[1]*lam5*r_hat/(2.0*t_hat*rB2a2);
		val_absPeJac[2][1] = -val_normal[0]*val_normal[1]*lam2 - r_hat*val_normal[1]*val_normal[0]*lam4/(2.0*t_hat) + s_hat*val_normal[0]*val_normal[1]*lam5/(2.0*t_hat);
		val_absPeJac[2][2] = val_normal[0]*val_normal[0]*lam2 + val_normal[2]*val_normal[2]*lam3 -r_hat*val_normal[1]*val_normal[1]*lam4/(2.0*t_hat) + s_hat*val_normal[1]*val_normal[1]*lam5/(2.0*t_hat);
		val_absPeJac[2][3] = -val_normal[2]*val_normal[1]*lam2 - r_hat*val_normal[2]*val_normal[1]*lam4/(2.0*t_hat) + s_hat*lam5*val_normal[1]*val_normal[2]/(2.0*t_hat);
		val_absPeJac[2][4] = 0.0;
    
		val_absPeJac[3][0] = r_hat*s_hat*val_normal[2]*lam4/(2.0*t_hat*rB2a2) - r_hat*s_hat*val_normal[2]*lam5/(2.0*t_hat*rB2a2);
		val_absPeJac[3][1] = -val_normal[0]*val_normal[2]*lam3 - lam4*val_normal[0]*val_normal[2]*r_hat/(2.0*t_hat) + lam5*val_normal[0]*val_normal[2]*s_hat/(2.0*t_hat);
		val_absPeJac[3][2] = -val_normal[1]*val_normal[2]*lam3 - lam4*val_normal[1]*val_normal[2]*r_hat/(2.0*t_hat) + lam5*val_normal[1]*val_normal[2]*s_hat/(2.0*t_hat);
		val_absPeJac[3][3] = (val_normal[1]*val_normal[1] + val_normal[0]*val_normal[0])*lam3 - lam4*val_normal[2]*val_normal[2]*r_hat/(2.0*t_hat) + lam5*val_normal[2]*val_normal[2]*s_hat/(2.0*t_hat);
		val_absPeJac[3][4] = 0.0;
    
		val_absPeJac[4][0] = 0.0;
		val_absPeJac[4][1] = 0.0;
		val_absPeJac[4][2] = 0.0;
		val_absPeJac[4][3] = 0.0;
		val_absPeJac[4][4] = lam1;
    
	}
  
}

void CNumerics::GetJacInviscidLambda_fabs(double *val_velocity, double val_soundspeed,
                                          double *val_normal, double *val_Lambda_Vector) {
	double ProjVelocity = 0;
  
	for (unsigned short iDim = 0; iDim < nDim; iDim++)
  ProjVelocity += val_velocity[iDim]*val_normal[iDim];
  
	if (nDim == 3) {
		val_Lambda_Vector[0] = fabs(ProjVelocity);
		val_Lambda_Vector[1] = fabs(ProjVelocity);
		val_Lambda_Vector[2] = fabs(ProjVelocity);
		val_Lambda_Vector[3] = fabs(ProjVelocity + val_soundspeed);
		val_Lambda_Vector[4] = fabs(ProjVelocity - val_soundspeed);
	}
  
	if(nDim == 2) {
		val_Lambda_Vector[0] = fabs(ProjVelocity);
		val_Lambda_Vector[1] = fabs(ProjVelocity);
		val_Lambda_Vector[2] = fabs(ProjVelocity + val_soundspeed);
		val_Lambda_Vector[3] = fabs(ProjVelocity - val_soundspeed);
	}
}

void CNumerics::GetViscousFlux(double *val_primvar, double **val_gradprimvar,
                               double val_laminar_viscosity, double val_eddy_viscosity, double val_mach_inf) {
  
	double total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	double cp = (Gamma / Gamma_Minus_One) * Gas_Constant;
	double heat_flux_factor = cp * (val_laminar_viscosity/PRANDTL + val_eddy_viscosity/PRANDTL_TURB);
  
	double div_vel = 0.0;
	for (unsigned short iDim = 0 ; iDim < nDim; iDim++)
  div_vel += val_gradprimvar[iDim+1][iDim];
  
	for (unsigned short iDim = 0 ; iDim < nDim; iDim++) {
		for (unsigned short jDim = 0 ; jDim < nDim; jDim++) {
			tau[iDim][jDim] = total_viscosity*( val_gradprimvar[jDim+1][iDim] +
                                         val_gradprimvar[iDim+1][jDim] )
      -TWO3*total_viscosity*div_vel*delta[iDim][jDim];
		}
	}
  
	// Gradient of primitive variables -> [Temp vel_x vel_y vel_z Pressure]
	if (nDim == 3) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][2];
		Flux_Tensor[4][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2] + tau[0][2]*val_primvar[3] +
    heat_flux_factor*val_gradprimvar[0][0];
    
		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][2];
		Flux_Tensor[4][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2] + tau[1][2]*val_primvar[3] +
    heat_flux_factor*val_gradprimvar[0][1];
    
		Flux_Tensor[0][2] = 0.0;
		Flux_Tensor[1][2] = tau[2][0];
		Flux_Tensor[2][2] = tau[2][1];
		Flux_Tensor[3][2] = tau[2][2];
		Flux_Tensor[4][2] = tau[2][0]*val_primvar[1] + tau[2][1]*val_primvar[2] + tau[2][2]*val_primvar[3] +
    heat_flux_factor*val_gradprimvar[0][2];
	}
	if (nDim == 2) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2]+
    heat_flux_factor*val_gradprimvar[0][0];
    
		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2]+
    heat_flux_factor*val_gradprimvar[0][1];
	}
}

void CNumerics::GetViscousProjFlux(double *val_primvar,
                                   double **val_gradprimvar, double val_turb_ke,
                                   double *val_normal,
                                   double val_laminar_viscosity,
                                   double val_eddy_viscosity) {
  
	unsigned short iVar, iDim, jDim;
	double total_viscosity, heat_flux_factor, div_vel, cp, Density;
	Density = val_primvar[nDim+2];
  
	total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	cp = (Gamma / Gamma_Minus_One) * Gas_Constant;
	heat_flux_factor = cp * (val_laminar_viscosity/PRANDTL + val_eddy_viscosity/PRANDTL_TURB);
  
	div_vel = 0.0;
	for (iDim = 0 ; iDim < nDim; iDim++)
  div_vel += val_gradprimvar[iDim+1][iDim];
  
	for (iDim = 0 ; iDim < nDim; iDim++)
  for (jDim = 0 ; jDim < nDim; jDim++)
  tau[iDim][jDim] = total_viscosity*( val_gradprimvar[jDim+1][iDim] + val_gradprimvar[iDim+1][jDim] )
  - TWO3*total_viscosity*div_vel*delta[iDim][jDim]
  - TWO3*Density*val_turb_ke*delta[iDim][jDim];
  
  
	/*--- Gradient of primitive variables -> [Temp vel_x vel_y vel_z Pressure] ---*/
	if (nDim == 2) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2]+
    heat_flux_factor*val_gradprimvar[0][0];
    
		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2]+
    heat_flux_factor*val_gradprimvar[0][1];
	} else {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][2];
		Flux_Tensor[4][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2] + tau[0][2]*val_primvar[3] +
    heat_flux_factor*val_gradprimvar[0][0];
    
		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][2];
		Flux_Tensor[4][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2] + tau[1][2]*val_primvar[3] +
    heat_flux_factor*val_gradprimvar[0][1];
    
		Flux_Tensor[0][2] = 0.0;
		Flux_Tensor[1][2] = tau[2][0];
		Flux_Tensor[2][2] = tau[2][1];
		Flux_Tensor[3][2] = tau[2][2];
		Flux_Tensor[4][2] = tau[2][0]*val_primvar[1] + tau[2][1]*val_primvar[2] + tau[2][2]*val_primvar[3] +
    heat_flux_factor*val_gradprimvar[0][2];
	}
  
	for (iVar = 0; iVar < nVar; iVar++) {
		Proj_Flux_Tensor[iVar] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
    Proj_Flux_Tensor[iVar] += Flux_Tensor[iVar][iDim] * val_normal[iDim];
	}
  
}

void CNumerics::GetViscousProjJacs(double *val_Mean_PrimVar, double val_laminar_viscosity,
                                   double val_eddy_viscosity, double val_dist_ij, double *val_normal, double val_dS,
                                   double *val_Proj_Visc_Flux, double **val_Proj_Jac_Tensor_i, double **val_Proj_Jac_Tensor_j) {
	unsigned short iDim, iVar, jVar;
  
	double theta = 0.0, sqvel = 0.0, proj_viscousflux_vel = 0.0;
  
	for (iDim = 0; iDim < nDim; iDim++) {
		theta += val_normal[iDim]*val_normal[iDim];
		sqvel += val_Mean_PrimVar[iDim+1]*val_Mean_PrimVar[iDim+1];
		proj_viscousflux_vel += val_Proj_Visc_Flux[iDim+1]*val_Mean_PrimVar[iDim+1];
	}
  
	double phi = 0.5*(Gamma-1.0)*sqvel;
	double Density = val_Mean_PrimVar[nDim+2];
	double Pressure = val_Mean_PrimVar[nDim+1];
	double total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	double heat_flux_factor = val_laminar_viscosity / PRANDTL + val_eddy_viscosity / PRANDTL_TURB;
	double cpoR = Gamma/(Gamma-1.0); // cp over R
	double factor = total_viscosity*val_dS/(Density*val_dist_ij);
	double phi_rho = -cpoR*heat_flux_factor*Pressure/(Density*Density);
	double phi_p = cpoR*heat_flux_factor/(Density);
	double rhoovisc = Density/(total_viscosity); // rho over viscosity
  
  for (unsigned short iVar = 0; iVar < nVar; iVar++) {
    for (unsigned short jVar = 0; jVar < nVar; jVar++) {
      val_Proj_Jac_Tensor_i[iVar][jVar] = 0.0;
      val_Proj_Jac_Tensor_j[iVar][jVar] = 0.0;
    }
  }
  
	if (nDim == 2) {
    
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
    
		double etaz = val_normal[0]*val_normal[1]/3.0;
    
		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay;
    
		val_Proj_Jac_Tensor_i[0][0] = 0.0;
		val_Proj_Jac_Tensor_i[0][1] = 0.0;
		val_Proj_Jac_Tensor_i[0][2] = 0.0;
		val_Proj_Jac_Tensor_i[0][3] = 0.0;
		val_Proj_Jac_Tensor_i[1][0] = factor*pix;
		val_Proj_Jac_Tensor_i[1][1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[1][2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[1][3] = 0.0;
		val_Proj_Jac_Tensor_i[2][0] = factor*piy;
		val_Proj_Jac_Tensor_i[2][1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[2][3] = 0.0;
    
		val_Proj_Jac_Tensor_i[3][0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) - (pix*val_Mean_PrimVar[1]+piy*val_Mean_PrimVar[2]));
		val_Proj_Jac_Tensor_i[3][1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1.0)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[3][2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1.0)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[3][3] = -factor*((Gamma-1.0)*rhoovisc*theta*phi_p);
    
		for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
    val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];
    
		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[3][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[3][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[3][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[3][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[3][2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[3][2] += factor*val_Proj_Visc_Flux[2];
    
    
	}
	else {
    
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
		double thetaz = theta + val_normal[2]*val_normal[2]/3.0;
    
		double etax = val_normal[1]*val_normal[2]/3.0;
		double etay = val_normal[0]*val_normal[2]/3.0;
		double etaz = val_normal[0]*val_normal[1]/3.0;
    
		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz   + val_Mean_PrimVar[3]*etay;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay + val_Mean_PrimVar[3]*etax;
		double piz = val_Mean_PrimVar[1]*etay   + val_Mean_PrimVar[2]*etax   + val_Mean_PrimVar[3]*thetaz;
    
		val_Proj_Jac_Tensor_i[0][0] = 0.0;
		val_Proj_Jac_Tensor_i[0][1] = 0.0;
		val_Proj_Jac_Tensor_i[0][2] = 0.0;
		val_Proj_Jac_Tensor_i[0][3] = 0.0;
		val_Proj_Jac_Tensor_i[0][4] = 0.0;
		val_Proj_Jac_Tensor_i[1][0] = factor*pix;
		val_Proj_Jac_Tensor_i[1][1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[1][2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[1][3] = -factor*etay;
		val_Proj_Jac_Tensor_i[1][4] = 0.0;
		val_Proj_Jac_Tensor_i[2][0] = factor*piy;
		val_Proj_Jac_Tensor_i[2][1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[2][3] = -factor*etax;
		val_Proj_Jac_Tensor_i[2][4] = 0.0;
		val_Proj_Jac_Tensor_i[3][0] = factor*piz;
		val_Proj_Jac_Tensor_i[3][1] = -factor*etay;
		val_Proj_Jac_Tensor_i[3][2] = -factor*etax;
		val_Proj_Jac_Tensor_i[3][3] = -factor*thetaz;
		val_Proj_Jac_Tensor_i[3][4] = 0.0;
		val_Proj_Jac_Tensor_i[4][0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) - (pix*val_Mean_PrimVar[1] + piy*val_Mean_PrimVar[2] + piz*val_Mean_PrimVar[3]));
		val_Proj_Jac_Tensor_i[4][1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[4][2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[4][3] = -factor*(piz-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[3]);
		val_Proj_Jac_Tensor_i[4][4] = -factor*((Gamma-1)*rhoovisc*theta*phi_p);
    
		for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
    val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];
    
		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[4][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[4][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[4][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[4][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[4][2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[4][2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_i[4][3] += factor*val_Proj_Visc_Flux[3];
		val_Proj_Jac_Tensor_j[4][3] += factor*val_Proj_Visc_Flux[3];
    
	}
  
}

void CNumerics::CreateBasis(double *val_Normal) {
  unsigned short iDim;
  double modm, modl;
  
  /*--- Define l as a vector in the plane normal to the supplied vector ---*/
  l[0] = 0.0;
  l[1] = -val_Normal[2];
  l[2] = val_Normal[1];
  
  /*--- Check for the zero vector and re-assign if needed ---*/
  if (l[0] == 0.0 && l[1] == 0.0 && l[2] == 0.0) {
    l[0] = -val_Normal[2];
    l[1] = 0.0;
    l[2] = val_Normal[0];
  }
  
  /*--- Take vector product of n * l to make m ---*/
  m[0] = val_Normal[1]*l[2] - val_Normal[2]*l[1];
	m[1] = val_Normal[2]*l[0] - val_Normal[0]*l[2];
	m[2] = val_Normal[0]*l[1] - val_Normal[1]*l[0];
  
  /*--- Normalize ---*/
  modm =0 ; modl = 0;
  for (iDim =0 ; iDim < nDim; iDim++) {
    modm += m[iDim]*m[iDim];
    modl += l[iDim]*l[iDim];
  }
  modm = sqrt(modm);
  modl = sqrt(modl);
  for (iDim =0 ; iDim < nDim; iDim++) {
    l[iDim] = l[iDim]/modl;
    m[iDim] = m[iDim]/modm;
  }
}

CSourceNothing::CSourceNothing(unsigned short val_nDim, unsigned short val_nVar, CConfig *config) : CNumerics(val_nDim, val_nVar, config) { }

CSourceNothing::~CSourceNothing(void) { }
