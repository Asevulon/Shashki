#include"pch.h"
#include"Neuron.h"
double rand(double left, double right)
{
	return left + (right - left) * double(rand()) / double(RAND_MAX);
}


neuron::neuron(int size)
{
	w = vector<double>(size, 0);
	for (int i = 0; i < size; i++)
	{
		w[i] = rand(-0.2, 0.2);
	}
	fi = rand(-0.2, 0.2);
}

NW& NW::operator =(const NW& nw)
{
	Layer = nw.Layer;
	return *this;
}

NW::NW(vector<int>& LS, int Enters)
{
	ls = LS;
	if (ls.empty())return;
	Layer.push_back(vector<neuron>());
	for (int i = 0; i < LS[0]; i++)
	{
		Layer[0].push_back(neuron(Enters));
	}
	for (int i = 1; i < LS.size(); i++)
	{
		Layer.push_back(vector<neuron>());
		for (int j = 0; j < LS[i]; j++)
		{
			Layer[i].push_back(neuron(LS[i - 1]));
		}
	}

	for (int i = 0; i < 32; i++)
	{
		Layer[Layer.size() - 1][0].w.push_back(1);
	}
}


void NW::ProcessNeuron(neuron& n, vector<double>& in, int len)
{
	double g_res = 0;

	for (int i = 0; i < len; i++)
	{
		g_res += n.w[i] * in[i];
	}
	g_res -= n.fi;
	n.out = f(g_res);
}
vector<double>Tin(42, 0);


template<typename T>
double NW::Calc(vector<T>& in)
{
	int g_len = 0;

	for (int i = 0; i < in.size(); i++)Tin[i] = in[i];
	g_len = in.size();
	for (int i = 0; i < Layer.size() - 1; i++)
	{
		for (int j = 0; j < Layer[i].size(); j++)
		{
			ProcessNeuron(Layer[i][j], Tin, g_len);
		}
		for (int j = 0; j < Layer[i].size(); j++)
		{
			Tin[j] = Layer[i][j].out;
		}
		g_len = Layer[i].size();
	}

	for (int i = 10; i < 42; i++)
	{
		Tin[i] = in[i - 10];
	}
	ProcessNeuron(Layer[2][0], Tin, 42);

	return Layer[2][0].out;
}

double NW::f(double x)
{
	return 1. / (1. + exp(-2 * x));
}

inline NW::PossibleTurn NW::Max(vector<PossibleTurn>& p)
{
	PossibleTurn res = p[0];
	for (int i = 1; i < p.size(); i++)
	{
		if (p[i].est > res.est)res = p[i];
	}
	return res;
}

inline NW::PossibleTurn NW::Min(vector<PossibleTurn>& p)
{
	PossibleTurn res = p[0];
	for (int i = 0; i < p.size(); i++)
	{
		if (p[i].est < res.est)res = p[i];
	}
	return res;
}


void NW::MakeChild(NW& left, NW& right)
{
	double chance = 0;
	for (int i = 0; i < Layer.size(); i++)
	{
		for (int j = 0; j < Layer[i].size(); j++)
		{
			for (int k = 0; k < Layer[i][j].w.size(); k++)
			{
				chance = rand(0, 1);
				Layer[i][j].w[k] = chance * left.Layer[i][j].w[k] + (1 - chance) * right.Layer[i][j].w[k];
			}
			chance = rand(0, 1);
			Layer[i][j].fi = chance * left.Layer[i][j].fi + (1 - chance) * right.Layer[i][j].fi;
		}
	}
}

void NW::Mutate()
{
	double M = MutationVal;
	for (int i = 0; i < Layer.size() - 1; i++)
	{
		for (int j = 0; j < Layer[i].size(); j++)
		{
			for (int k = 0; k < Layer[i][j].w.size(); k++)
			{
				Layer[i][j].w[k] *= rand(1 - M, 1 + M);
			}
			Layer[i][j].fi *= rand(1 - M, 1 + M);
		}
	}

	int last = Layer.size() - 1;
	for (int k = 0; k < 10; k++)
	{
		Layer[last][0].w[k] *= rand(1 - M, 1 + M);
	}
	Layer[last][0].fi *= rand(1 - M, 1 + M);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


Trainer::Trainer()
{
	srand(time(NULL));


	for (int i = 0; i < _size; i++)
	{
		NW nw1(LS, 32);
		P.push_back(nw1);
	}
}



pair<ip, ip> NW::MakePredictions(Game& in)
{
	auto pt = in.GetMoveable();
	if (pt.empty())
	{
		return pair<ip,ip>(ip(-1, -1), ip(-1, -1));
	}
	vector<PossibleTurn> p;
	for (int i = 0; i < pt.size(); i++)
	{
		vector<ip>outs;
		in.Select(pt[i].first, pt[i].second, outs);
		for (int j = 0; j < outs.size(); j++)
		{
			p.push_back(NeuronMinmax1(in, pt[i], outs[j]));
		}
	}

	auto res = Max(p);
	return pair<ip, ip>(res.from, res.to);
}

inline NW::PossibleTurn NW::NeuronMinmax1(Game in, ip& from, ip& to)
{
	if (!in.DoTurn(to.first, to.second))abort();
	auto pt = in.GetMoveable();
	if (pt.empty())
	{
		PossibleTurn res;
		res.from = from;
		res.to = to;
		res.est = INT_MIN;
		return res;
	}
	vector<PossibleTurn> p;

	for (int i = 0; i < pt.size(); i++)
	{
		vector<ip>outs;
		in.Select(pt[i].first, pt[i].second, outs);
		for (int j = 0; j < outs.size(); j++)
		{
			p.push_back(NeuronMinmax2(in, pt[i], outs[j]));
		}
	}

	return Min(p);
}
inline NW::PossibleTurn NW::NeuronMinmax2(Game in, ip& from, ip& to)
{
	if (!in.DoTurn(to.first, to.second))abort();
	auto pt = in.GetMoveable();
	if (pt.empty())
	{
		PossibleTurn res;
		res.from = from;
		res.to = to;
		res.est = INT_MIN;
		return res;
	}
	vector<PossibleTurn> p;

	for (int i = 0; i < pt.size(); i++)
	{
		vector<ip>outs;
		in.Select(pt[i].first, pt[i].second, outs);
		for (int j = 0; j < outs.size(); j++)
		{
			p.push_back(Estimate(in, pt[i], outs[j]));
		}
	}

	return Max(p);
}

inline NW::PossibleTurn NW::Estimate(Game in, ip& from, ip& to)
{
	if (!in.DoTurn(to.first, to.second))abort();
	PossibleTurn res;
	res.est = Calc(in.GetBoardN());
	res.from = from;
	res.to = to;
	return res;
}

void NW::Save()
{
	ofstream ofstr("best.nw");
	
	ofstr << Layer.size() << endl;

	for (auto& layer : Layer)
	{
		ofstr << layer.size() << endl;
		for (auto& n : layer)
		{
			ofstr << n.w.size() << endl;
			for (auto& ww : n.w)
			{
				ofstr << ww << endl;
			}
			ofstr << n.fi << endl;
		}
	}

	ofstr.close();
}

void NW::Load()
{
	ifstream ifstr("best.nw");
	
	int layernum = 0;
	ifstr >> layernum;
	Layer.resize(layernum);

	for (int i = 0; i < layernum; i++)
	{
		int layersize = 0;
		ifstr >> layersize;
		Layer[i].resize(layersize);

		for (int j = 0; j < layersize; j++)
		{
			int wsize = 0;
			ifstr >> wsize;
			Layer[i][j].w.resize(wsize);
			
			for (int k = 0; k < wsize; k++)
			{
				ifstr >> Layer[i][j].w[k];
			}
			ifstr >> Layer[i][j].fi;
		}
	}

	ifstr.close();
}

void NW::Save(ofstream& ofstr)
{
	ofstr << Layer.size() << endl;

	for (auto& layer : Layer)
	{
		ofstr << layer.size() << endl;
		for (auto& n : layer)
		{
			ofstr << n.w.size() << endl;
			for (auto& ww : n.w)
			{
				ofstr << ww << endl;
			}
			ofstr << n.fi << endl;
		}
	}
}

void NW::Load(ifstream& ifstr)
{
	int layernum = 0;
	ifstr >> layernum;
	Layer.resize(layernum);

	for (int i = 0; i < layernum; i++)
	{
		int layersize = 0;
		ifstr >> layersize;
		Layer[i].resize(layersize);

		for (int j = 0; j < layersize; j++)
		{
			int wsize = 0;
			ifstr >> wsize;
			Layer[i][j].w.resize(wsize);

			for (int k = 0; k < wsize; k++)
			{
				ifstr >> Layer[i][j].w[k];
			}
			ifstr >> Layer[i][j].fi;
		}
	}
}


void Trainer::score(NW& p1, NW& p2, bool turn)
{
	Game game;
	if (turn)
		while (!game.IsGameEnd())
		{
			auto pos = p1.MakePredictions(game);
			if (game.IsGameEnd())break;

			vector<ip>out;
			game.Select(pos.first.first, pos.first.second, out);
			game.DoTurn(pos.second.first, pos.second.second);

			pos = p2.MakePredictions(game);
			if (game.IsGameEnd())break;

			vector<ip>out2;
			game.Select(pos.first.first, pos.first.second, out2);
			game.DoTurn(pos.second.first, pos.second.second);
		}
	else
		while (!game.IsGameEnd())
		{
			auto pos = p2.MakePredictions(game);
			if (game.IsGameEnd())break;

			vector<ip>out;
			game.Select(pos.first.first, pos.first.second, out);
			game.DoTurn(pos.second.first, pos.second.second);

			pos = p1.MakePredictions(game);
			if (game.IsGameEnd())break;

			vector<ip>out2;
			game.Select(pos.first.first, pos.first.second, out2);
			game.DoTurn(pos.second.first, pos.second.second);
		}

	int winner = game.Winner();
	if ((winner == 1) && turn)
	{
		p1.score += WIN;
		return;
	}
	if ((winner == 0) && turn)
	{
		p1.score += LOSE;
		return;
	}

	if ((winner == 0) && (!turn))
	{
		p1.score += WIN;
		return;
	}
	if ((winner == 1) && (!turn))
	{
		p1.score += LOSE;
		return;
	}

	if ((winner == 1) && turn)
	{
		p1.score += WIN;
		return;
	}
	if (winner == -1)
	{
		p1.score += DRAW;
		return;
	}
}

inline void Trainer::ResetScore()
{
	for (int i = 0; i < _size; i++)P[i].score = 0;
}

inline void Trainer::ScoreAll()
{
	bool turn = (int)rand(0.5, 1.5);
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _games; j++)
		{
			int id = rand(0, _size - 1e-6);
			while (id == i)id = rand(0, _size - 1e-6);
			score(P[i], P[id], turn);
		}
		turn = !turn;
	}
}

inline void Trainer::Replace()
{
	for (int i = _size / 2; i < _size; i++)
	{
		double ver = rand(0, 1);
		if (ver < KillVer)P[i] = NW(LS, 32);
		else
		{
			int id1 = rand(0, _size / 2 - 1e-6);
			int id2 = rand(0, _size / 2 - 1e-6);
			P[i].MakeChild(P[id1], P[id2]);
		}
	}
}

inline void Trainer::MutateAll()
{
	for (int i = 0; i < _size; i++)
	{
		double ver = rand(0, 1);
		if (ver < MutVer)P[i].Mutate();
	}
}



bool operator<(NW const& a, NW const& b)
{
	return a.score < b.score;
}
void Trainer::train()
{
	int counter = 0;
	bool turn = true;
	while (1)
	{
		ResetScore();
		
		ScoreAll();

		sort(P.begin(), P.end());
		BestScore = P[0].score;
		if (P[0].score = 5)
		{
			if (OutNW != nullptr)delete OutNW;
			OutNW = new NW(P[0]);
			break;
		}

		Replace();
		
		MutateAll();

		counter++;
		if (counter > TrainLimit)break;
	}
}

void Trainer::SaveBest()
{
	sort(P.begin(), P.end());
	P[0].Save();
}

void Trainer::SaveAll()
{
	ofstream ofstr("all.nw");
	for (int i = 0; i < _size; i++)
	{
		P[i].Save(ofstr);
	}
	ofstr.close();
}

void Trainer::Load()
{
	if (OutNW != nullptr)delete OutNW;
	OutNW = new NW(vector<int>(), 0);
	OutNW->Load();
}

void Trainer::LoadALL()
{
	ifstream ifstr("all.nw");
	for (int i = 0; i < _size; i++)
	{
		P[i].Load(ifstr);
	}
	ifstr.close();
}



