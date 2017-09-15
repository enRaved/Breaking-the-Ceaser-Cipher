#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#define N 26
#define M 26
#define T 300



static double A[N][N];

static double B[N][M];


static double bestB[N][M];
static double bestPi[N];


static double pi[N];
static int maxIters;

static int observationSequence[T];
static double C[T];
static double alpha[T][N];
static double beta[T][N];
static double mygamma[T][N];
static double digamma[T][N][N];
static double bestScore;

int iters = 0;
double oldLogProb = -DBL_MAX, logProb;

void computeAlphaPass();
void computeBetaPass();
void computeGammaDigamma();
void reestimate();
void initialiseGreekLetters();

void printGreekLetter(double [][N]);

void readFileToPlainText(char *, char *);
void encryptText(char *, char *);
int convertLettersToNumbers(char);
char convertNumbersToLetters(int);
void printSequence(int []);

void generateDiagraph();
void normalizeA();
void assignBestModel();

int main(int argc, char *argv[]){
int numberOfHmms = 2;
int number = 0;
	maxIters = atoi(argv[3]);
	int seed = atoi(argv[4]);


	/* read from file and encrypt text 
	encrypted text goes in the observation sequence
	*/

	char *filename = NULL;
	char *plaintext = NULL, *encryptedText = NULL;
	filename = "brown.txt";
	plaintext = (char *)malloc(sizeof(char *) * (T));
	encryptedText = (char *)malloc(sizeof(char *) * (strlen(plaintext)));
	char *p, *e;
	p = plaintext;
	
	readFileToPlainText(filename,p);
	//printf("\nPlaintext: %s", plaintext);
	p = plaintext;
	e = encryptedText;
	encryptText(p,e);


	generateDiagraph();
    normalizeA();

bestScore = -DBL_MAX;

while (number < numberOfHmms){

	iters = 0;
	logProb = 0;

	initialiseGreekLetters();


	int k = 0;
	int c;
	iters = 0;
	
	//T = (long) atoi(argv[2]);


	//initilaise A with the diagraph values



	//initialise B randomly and make row stochastic
	
	double tempB[N][M];
	for(int i = 0; i < N; i++){
		double sum = 0;
		for(int j = 0; j < M; j++){

			double diff = 0.7 - 0.3;
			tempB[i][j] = (((float) rand() / RAND_MAX) * diff) + 0.3;
			tempB[i][j] += 5;
			sum += tempB[i][j];	
		}

		for(int k = 0; k < M; k++){
			B[i][k] = tempB[i][k] / sum;
		}
	}



		// //print B
		// printf("\nB:");
		// for(int i = 0; i < N; i++){
		// 	printf("\n");
		// 	for(int j = 0; j < M; j++){
		// 		printf("%f ", B[i][j]);
		// 	}
		// }

	//initialise pi
	
	
	double sum = 0;
	double tempPi[N];
	for(int j = 0; j < N; j++){
			
			double diff = 0.6 - 0.4;
			tempPi[j] = (((float) rand() / RAND_MAX) * diff) + 0.4;
			tempPi[j] += 5;
			sum += tempPi[j];	
		}
	for(int j = 0; j < N; j++){
			pi[j] = tempPi[j] / sum;
		}

// //print Pi
// 		printf("Print PI \n");
// 		for(int i = 0; i < N; i++){
// 			printf("%lf ", pi[i]);
// 		}



	//print observation sequence
	// for (int i = 0; i < T; ++i)
	// {
	// 	printf("%d\t%d\n", i, observationSequence[i]);
	// }


while(iters < maxIters || logProb > oldLogProb){

	computeAlphaPass();
	computeBetaPass();
	computeGammaDigamma();

	reestimate();

	// 6. Compute Compute log[P (O | λ)]
	logProb = 0;
	for(int i = 0; i < T; i++){
		logProb = logProb + log (C[i]);
	}
	logProb = -logProb;

	//To iterate or not to iterate, that is the question. . .
	

	iters = iters + 1;

	

	//if(iters < maxIters && logProb > oldLogProb){
	if(iters < maxIters){
		oldLogProb = logProb;
		
	}
	else{
		//print pi
		// printf("\n Final Pi");
		// for(int i = 0; i < N; i++){
		// 	printf("\t%f", pi[i]);
		// }
		// //print A
		// printf("\nFinal A:");
		// for(int i = 0; i < N; i++){
		// 	printf("\n");
		// 	for(int j = 0; j < N; j++){
		// 		printf("%f\t", A[i][j]);
		// 	}
		// }

		printf("Interation %d: %f\n", iters, logProb);
		if(bestScore < logProb){
			bestScore = logProb;
			assignBestModel();
			}



		printf("\n");
		//print B
		// printf("\nFinal B:");
		// for(int i = 0; i < N; i++){
		// 	printf("\n");
		// 	for(int j = 0; j < M; j++){
		// 		printf("%f ", B[i][j]);
		// 	}
		// }
		printf("\n");

		printf("\n");
		//print B
		// printf("\nKeys for encrypted a, b c,...z:");
		// for(int i = 0; i < N; i++){
		// 	printf("\n");
		// 	double max = 0;
		// 	int maxj;
		// 	for(int j = 0; j < M; j++){
		// 		if( max  < B[i][j]){
		// 			max = B[i][j];
		// 			maxj = j;
		// 		}
					
		// 		}
		// 		printf("%d : %lf\n", maxj, max );
				
			
		// }
		printf("\n");


		break;
	}


} //while ends here


number++;
printf("Score for %d hmm: %lf\n", number, logProb);

}
printf("Best score: %lf\n", logProb);
printf("\nB OF BEST MODEL\n");
for(int i = 0; i < N; i++){
	printf("\n");
	for(int j = 0; j < M; j++){
		printf("%lf ",bestB[i][j]);
	}
}

		printf("\nShifted Keys for encrypted a, b c,...z:");
		for(int i = 0; i < N; i++){
			printf("\n");
			double max = 0;
			int maxj = 0;
			for(int j = 0; j < M; j++){
				if( max  < bestB[i][j]){
					max = bestB[i][j];
					maxj = j;
				}
					
				}
				printf("%d : %lf\n", maxj, max );
				
			
		}


}

void computeAlphaPass(){

	
	// compute α0(i)
		C[0] = 0;
		for(int i = 0; i < N; i++){
		alpha[0][i] = pi[i] * B[i][observationSequence[0]];
		C[0] = C[0] + alpha[0][i];
	}

	//scale the α0(i)
	C[0] = 1/C[0];
	for(int i = 0; i < N; i++){
		alpha[0][i] = C[0] * alpha[0][i];
	}

	//compute αt(i)
	for(int t = 1; t < T; t++){
		C[t] = 0;
		for(int i = 0; i < N; i++){
			alpha[t][i] = 0;
			for(int j = 0; j < N; j++){
				alpha[t][i] = alpha[t][i] + alpha[t-1][j] * A[j][i];
			}
			alpha[t][i] = alpha[t][i] * B[i][observationSequence[t]];
			C[t] = C[t] + alpha[t][i];
		}
		// scale αt(i)
		C[t] = 1/C[t];
		for(int i = 0; i < N; i++){
			alpha[t][i] = C[t] * alpha[t][i];
		}
	}

}

void computeBetaPass(){
	for(int i = 0; i < N; i++){
		beta[T-1][i] = C[T-1];
	}

	// β-pass
	for(int t = (T - 2); t >= 0; t--){
		for(int i = 0; i < N; i++){
			beta[t][i] = 0;
			for(int j = 0; j < N; j++){
				beta[t][i] = beta[t][i] + A[i][j] * B[j][observationSequence[t+1]] * beta[t+1][j];
			}
			// scale βt(i) with same scale factor as αt(i)
			beta[t][i] = C[t] * beta[t][i];
		}
	}
}

void computeGammaDigamma(){
	double numer, denom;
		for(int t = 0; t < T-1; t++){
		denom = 0;
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				denom = denom + alpha[t][i] * A[i][j] * B[j][observationSequence[t+1]] * beta[t+1][j];
			}
		}
		for(int i = 0; i < N; i++){
			mygamma[t][i] = 0;

			for(int j = 0; j < N; j++){

				digamma[t][i][j] = ( alpha[t][i] * A[i][j] * B[j][observationSequence[t+1]] * beta[t+1][j] ) / denom;
				mygamma[t][i] = mygamma[t][i] + digamma[t][i][j];

			}
		}
	}

	// Special case for γT−1(i)
	denom = 0;
	for(int i = 0; i < N; i++){
		denom = denom + alpha[T-1][i];
	}
	for(int i = 0; i < N; i++){
		mygamma[T-1][i] = alpha[T-1][i]/denom;
	}
}

void reestimate(){
	// 5. Re-estimate A, B and π

	//re-estimate π
	for(int i = 0; i < N; i++){
		pi[i] = mygamma[0][i];
	}

	//do not reestimate A

	// //re-estimate A
	// double numer, denom;
	// for(int i = 0; i < N; i++){
	// 	for(int j = 0; j < N; j++){
	// 		numer = 0;
	// 		denom = 0;
	// 		for(int t  = 0; t < T-1; t++){
	// 			numer = numer + digamma[t][i][j];
	// 			denom = denom + mygamma[t][i];
	// 		}
	// 		A[i][j] = numer/denom;
	// 	}
	// }


	double numer,denom;
	//re-estimate B
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			numer = 0;
			denom = 0;
			for(int t = 0; t < T; t++){
				if(observationSequence[t]==j){
					numer = numer + mygamma[t][i];
				}
				denom = denom + mygamma[t][i];
			}
			B[i][j] = numer/denom;
		}
	}


}

void printGreekLetter(double greekletter[][N]){
//PRINT STATEMENT REMOVE
//print alpha, beta and gamma pass
printf("Pass\n");
for(int t = 0; t < T; t++){
	for(int i = 0; i < N; i++){
			printf("%lf\t", greekletter[t][i]);
		}
		printf("\n");
}
}

void readFileToPlainText(char *filename, char *plaintext){
	FILE *fr;
	char c;
	int k = 0;

	fr = fopen(filename, "r");

	if(fr == NULL){
		perror("\nError in opening file!");
	}

	if(fr){
		while((c=fgetc(fr))!=EOF && k < T){
			*plaintext = (char)c;
			//printf("%c\nPlaintext: \n", *plaintext);
			plaintext++;
 			//observationSequence[k] = convertLettersToNumbers(c);
 			k++;
 		}
	}
}

void encryptText(char *plaintext, char *encryptedText){
	int i = 0;
	while(*plaintext != '\0'){
		//printf("\n%c", *plaintext);
		//encrpyt and read into observation text
		observationSequence[i] = (convertLettersToNumbers(*plaintext)+1) % 26;
		i++;
		*encryptedText = convertNumbersToLetters((convertLettersToNumbers(*plaintext) + 1) % 26);
		plaintext++;
		//printf("\t%c\n", *encryptedText);
		encryptedText++;
	}

}

int convertLettersToNumbers(char c){
	//printf("%c\n", c);
		return c - 'a';
}

char convertNumbersToLetters(int number){
	//printf("in convertNumbersToLetters\n");
		//printf("%d\n",number);
		return (char)(number + 'a');
}

void printSequence(int observationSequence[]){

	for (int i = 0; i < T; ++i)
	{
		printf("%d\t", observationSequence[i]);
	}
}

void generateDiagraph(){

    int count['z' - 'a' + 1]['z' - 'a' + 1] = {{ 0 }};
    int char0 = EOF, c1, c2;
    int t = 0;
    FILE *file;
    file = fopen("brown.txt", "r");
    c1 = getc(file);

    if (file != NULL) {
        while ((c2 = getc(file)) != EOF && t < T) {
            count[c1 - 'a'][c2 - 'a']++;
            c1 = c2;
            t++;
        }
        fclose(file);
        for (c1 = 'a'; c1 <= 'z'; c1++) {
            for (c2 = 'a'; c2 <= 'z'; c2++) {
                int frequency = count[c1 - 'a'][c2 - 'a'];
                // formulate matrix
                A[convertLettersToNumbers(c1)][convertLettersToNumbers(c2)] = frequency;
            }
        }
    }

}

void normalizeA(){

    double sumOfRow[N];
        
        //add 5 to each element
    for(int i = 0; i < 26; i++){

        for(int j = 0; j < 26; j++){

           A[i][j] = A[i][j] + 5;

        }

    }

    double sum = 0;
    for(int i = 0; i < 26; i++){
         for(int j = 0; j < 26; j++){
            
            sum = sum + A[i][j];
        }
        sumOfRow[i] = sum;
        sum = 0;
    }

    for(int i = 0; i < N; i++){

        for(int j = 0; j < N; j++){

            A[i][j] = A[i][j]/sumOfRow[i];
        }
    }

}

void assignBestModel(){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			bestB[i][j] = B[i][j];
		}
	}

	for(int i = 0; i < N; i++){
		bestPi[i] = pi[i];
	}
}

void initialiseGreekLetters(){
		//initilise alpha
	for(int t = 0; t < T; t++){
		for(int i = 0; i < N; i++){
			alpha[t][i] = 0;
		}
	}

	//initilise beta
	for(int t = 0; t < T; t++){
		for(int i = 0; i < N; i++){
			beta[t][i] = 0;
		}
	}

	//initialise gamma
	for(int t = 0; t < T; t++){
		for(int i = 0; i < N; i++){
			mygamma[t][i] = 0;
		}
	}

	//initialise digamma
	for(int t = 0; t < T; t++){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				digamma[t][i][j] = 0;
			}
		}
	}
}

