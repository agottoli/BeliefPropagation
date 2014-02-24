
double* marginalizationBigN(size_t size, // dimTabCricca POW2
							size_t nArray, // dimTabSep POW2
							double *h_idata, // PSI da sommare
							size_t *h_iIndexData, // Indexig of PSI da sommare
							size_t dimInput, // dimensione vera cricca
							size_t dimRisultato // dimensione vera separatore
							, long long* elapsedSum, long long* elapsedDivMul
							);
double* marginalizationSmallN(size_t size, // dimTabCricca POW2
							size_t nArray, // dimTabSep POW2
							double *h_idata, // PSI da sommare
							size_t *h_iIndexData, // Indexig of PSI da sommare
							size_t dimInput, // dimensione vera cricca
							size_t dimRisultato // dimensione vera separatore
							, long long* elapsedSum, long long* elapsedDivMul
							);
void scattering(size_t size,  // dimTabCricca POW2
			size_t n, // dimTabSep POW2
			double *d_iVector1Data, // double *h_iVector2Data qui ci va il device_v1data
			double *h_iVector2Data, // FI
			double *h_MatrixData, // PSI da aggiornare
			size_t *h_MatrixIndex, // Indexig of PSI da aggiornare
			size_t dimCricca, // dimensione vera tabCricca
			size_t dimSeparatore // dim vera tabSep
			, long long* elapsedSum, long long* elapsedDivMul
			);

//void margAndScatt(size_t sizeCriccaPOW2, size_t sizeSeparatorePOW2, double *tabellaCricca, size_t *tabellaIndiciCricca, size_t dimCricca, size_t dimSeparatore, // dati per marginalization
//			size_t sizeCriccaMulPOW2, double* tabellaSeparatore, double* tabellaCriccaMul, size_t* tabellaIndiciCriccaMul, size_t dimCriccaMul // dati per scattering
//			);



void selectGPU();
void resetGPU();
