#pragma once

#include<afxwin.h>
#include<vector>

#include"Game.h"
using namespace std;

struct neuron
{
	double out = 0;

	vector<double> w;
	double fi = 0;

	neuron() {}
	neuron(int size);
};


class NW
{
private:
	vector<vector<neuron>>Layer;
	vector<int>LayerSize;

	vector<int>ls;
	const double MutationVal = 0.5;
protected:
	inline void ProcessNeuron(neuron& n, vector<double>& in, int len);
	inline double f(double x);
public:
	NW::NW(vector<int>& LS, int Enters);
	NW& NW::operator =(const NW& nw);
	template<typename T>
	inline double Calc(vector<T>& in);
	inline void NW::MakeChild(NW& left, NW& right);
	inline void NW::Mutate();
	inline pair<ip, ip> MakePredictions(Game& in, int turn);

	int score = 0;
	bool turn = true;
};



typedef  NW* link;
class Trainer
{
private:
	vector<NW>P;
	static const int _size = 30;
	static const int _games = 5;

	vector<int>LS = { 40,10, 1};
protected:
	void score(NW& target, NW& opponent);
public:
	int TrainLimit = 100000;
	Trainer();
	HWND parent;
	void GetBest(link& p1, link& p2);
	inline void score(NW& p1, NW& p2);
	void train();
	bool stop = false;
};

inline int Max(std::vector<double>& p);
