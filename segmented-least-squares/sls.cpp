#include <algorithm>
#include <vector>
#include <limits>
#include <iostream>
#include "omp.h"

using namespace std;

#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
typedef long long ll;
typedef pair<int, int> pii;
typedef vector<int> vi;
typedef vector<ll> vll;

#define pb push_back
#define lb lower_bound
#define ub upper_bound

#define INF numeric_limits<double>::infinity()


class Solver {
	vector<pii> points;
	int c;
	int n;
	vector<double> OPT;
	vll prefx, prefy, prefxy, prefxx;
	vector<vector<double> > slope, intercept, err;

public:
	void init(int N, vector<pii> &v, int C) {
		points.pb(pii(0,0)); // placeholder 
		for(auto &p : v) points.pb(p);
		
		n = N, c = C;
		OPT.assign(n+1, 0);
		prefx.assign(n+1, 0);
		prefy.assign(n+1, 0);
		prefxy.assign(n+1, 0);
		prefxx.assign(n+1, 0);
		slope.assign(n+1, vector<double>(n+1, 0));
		intercept.assign(n+1, vector<double>(n+1, 0));
		err.assign(n+1, vector<double>(n+1, 0));
	}
	
	void reset() {
		OPT.assign(n+1, 0);
		prefx.assign(n+1, 0);
		prefy.assign(n+1, 0);
		prefxy.assign(n+1, 0);
		prefxx.assign(n+1, 0);
		slope.assign(n+1, vector<double>(n+1, 0));
		intercept.assign(n+1, vector<double>(n+1, 0));
		err.assign(n+1, vector<double>(n+1, 0));
	}
	
	
	double sequential() {
		{ // calculate prefix sums
			ll a = 0;
			for(int i = 1; i <= n; ++i) {
				a += points[i].first;
				prefx[i] = a;
			}
			a = 0;
			for(int i = 1; i <= n; ++i) {
				a += points[i].second;
				prefy[i] = a;
			}
			a = 0;
			for(int i = 1; i <= n; ++i) {
				a += points[i].first * points[i].second;
				prefxy[i] = a;
			}
			a = 0;
			for(int i = 1; i <= n; ++i) {
				a += points[i].first * points[i].first;
				prefxx[i] = a;
			}
		}
		
		for(int i = 1; i <= n; ++i) {
			for(int j = i; j <= n; ++j) {
				int interval = j - i + 1;
				ll x_sum = prefx[j] - prefx[i-1];
				ll y_sum = prefy[j] - prefy[i-1];
				ll xy_sum = prefxy[j] - prefxy[i-1];
				ll xx_sum = prefxx[j] - prefxx[i-1];
				
				ll numerator = interval * xy_sum - x_sum * y_sum;
				
				if (numerator == 0)
					slope[i][j] = 0.0;
				else {
					ll denom = interval * xx_sum - x_sum * x_sum;
					slope[i][j] = (denom == 0) ? INF : (numerator / double(denom));				
				}
				intercept[i][j] = (y_sum - slope[i][j] * x_sum) / double(interval);
				
				err[i][j] = 0;
				
				for (int k = i; k <= j; ++k) {
					double tmp = points[k].second - slope[i][j] * points[k].first - intercept[i][j];
					err[i][j] += tmp * tmp;
				}
			}
		}
		
		for (int j = 1; j <= n; j++) {
			double small = INF;
			for (int i = 1; i <= j; i++) {
				double tmp = err[i][j] + OPT[i-1];
				small = min(small, tmp);
			}
			OPT[j] = small + c;
		}
		
		return OPT[n];
	}
	
	
	double parallel() {
		{ // calculate prefix sums
			ll a = 0;
			#pragma omp parallel for reduction (inscan, +:a)
			for(int i = 1; i <= n; ++i) {
				a += points[i].first;
				#pragma omp scan inclusive(a)
				prefx[i] = a;
			}
			a = 0;
			#pragma omp parallel for reduction (inscan, +:a)
			for(int i = 1; i <= n; ++i) {
				a += points[i].second;
				#pragma omp scan inclusive(a)
				prefy[i] = a;
			}
			a = 0;
			#pragma omp parallel for reduction (inscan, +:a)
			for(int i = 1; i <= n; ++i) {
				a += points[i].first * points[i].second;
				#pragma omp scan inclusive(a)
				prefxy[i] = a;
			}
			a = 0;
			#pragma omp parallel for reduction (inscan, +:a)
			for(int i = 1; i <= n; ++i) {
				a += points[i].first * points[i].first;
				#pragma omp scan inclusive(a)
				prefxx[i] = a;
			}
		}
		
		#pragma omp parallel for collapse(2)
		for(int i = 1; i <= n; ++i) {
			for(int j = i; j <= n; ++j) {
				int interval = j - i + 1;
				ll x_sum = prefx[j] - prefx[i-1];
				ll y_sum = prefy[j] - prefy[i-1];
				ll xy_sum = prefxy[j] - prefxy[i-1];
				ll xx_sum = prefxx[j] - prefxx[i-1];
				
				ll numerator = interval * xy_sum - x_sum * y_sum;
				
				if (numerator == 0)
					slope[i][j] = 0.0;
				else {
					ll denom = interval * xx_sum - x_sum * x_sum;
					slope[i][j] = (denom == 0) ? INF : (numerator / double(denom));				
				}
				intercept[i][j] = (y_sum - slope[i][j] * x_sum) / double(interval);
				
				err[i][j] = 0;
				
				#pragma omp parallel for reduction(+:err[i][j])
				for (int k = i; k <= j; ++k) {
					double tmp = points[k].second - slope[i][j] * points[k].first - intercept[i][j];
					err[i][j] += tmp * tmp;
				}
			}
		}
		
		
		for (int j = 1; j <= n; j++) {
			double small = INF;
			
			#pragma omp parallel for reduction(min:small)
			for (int i = 1; i <= j; i++) {
				double tmp = err[i][j] + OPT[i-1];
				small = min(small, tmp);
			}
			OPT[j] = small + c;
		}
		
		return OPT[n];
	}
	
};
