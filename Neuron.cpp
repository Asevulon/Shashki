#include"pch.h"
#include"Neuron.h"
#include<fstream>
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


struct PossibleTurn
{
	double est = 0;
	ip from = ip(0, 0);
	ip to = ip(0, 0);
};
pair<ip,ip> NW::MakePredictions(Game&in, int turn)
{
	auto pt = in.GetMoveable();
	vector<PossibleTurn> p;
	for (int i = 0; i < pt.size();i ++)
	{
		vector<ip>outs;
		in.Select(pt[i].first, pt[i].second, outs);
		{
			for (int j = 0; j < outs.size(); j++)
			{

			}
		}
	}
	
	return Max(res);
}

void Trainer::score(NW& p1, NW& p2)
{
	Game game;
	
	while (!game.IsGameEnd())
	{
		int pos = p1.MakePredictions()
	}
}



double avscore1 = 0;
double avscore2 = 0; 

void Trainer::train()
{
	int counter = 0;
	while (1)
	{
		avscore1 = 0;
		avscore2 = 0;
		for (int i = 0; i < _size; i++)
		{
			P[i].score = 0;
		}
		for (int i = 0; i < _size; i++)
		{
			for (int j = 0; j < _games; j++)
			{
				scorefirst(P1[i], P2[rand(0, _size - 1e-6)]);
				scoresecond(P1[rand(0, _size - 1e-6)], P2[i]);
				P1[i].avscore += P1[i].score;
				P2[i].avscore += P2[i].score;
			}
		}
		/*for (int i = 0; i < _size; i++)
		{
			for (int j = 0; j < _size; j++)
			{
				score(P1[i], P2[j]);
				P1[i].avscore += P1[i].score;
				P2[j].avscore += P2[j].score;
			}
		}*/
		/*thread thr1([this]()
			{
				for (int i = 0; i < _size / 4; i++)
				{
					for (int j = 0; j < _gamesvsrand; j++)
					{
						scoreVSrandom(P1[i], 1);
						P1[i].avscore += P1[i].score;

						scoreVSrandom(P2[i], -1);
						P2[i].avscore += P2[i].score;

					}
				}
			}
		); 
		thread thr2([this]()
			{
				for (int i = _size/4; i < _size / 2; i++)
				{
					for (int j = 0; j < _gamesvsrand; j++)
					{
						scoreVSrandom(P1[i], 1);
						P1[i].avscore += P1[i].score;

						scoreVSrandom(P2[i], -1);
						P2[i].avscore += P2[i].score;

					}
				}
			}
		);
		thread thr3([this]()
			{
				for (int i = _size/2; i < 3 * _size / 4; i++)
				{
					for (int j = 0; j < _gamesvsrand; j++)
					{
						scoreVSrandom(P1[i], 1);
						P1[i].avscore += P1[i].score;

						scoreVSrandom(P2[i], -1);
						P2[i].avscore += P2[i].score;

					}
				}
			}
		);
		for (int i = 3 * _size / 4; i < _size; i++)
		{
			for (int j = 0; j < _gamesvsrand; j++)
			{
				scoreVSrandom(P1[i], 1);
				P1[i].avscore += P1[i].score;

				scoreVSrandom(P2[i], -1);
				P2[i].avscore += P2[i].score;

			}
		}

		thr1.join();
		thr2.join();
		thr3.join();*/

		/*int step = _size / 8;
		thread* threads[16];
		for (int i = 0; i < 8; i++)
		{
			threads[i] = makethread(i * step, (i + 1) * step);
		}
		for (int i = 0; i < 8; i++)
		{
			threads[i]->join();
			delete threads[i];
		}*/

		for (int i = 0; i < _size; i++)
		{
			P1[i].avscore /= _gamesvsrand;
			P2[i].avscore /= _gamesvsrand;
			avscore1 += P1[i].avscore;
			avscore2 += P2[i].avscore;
		}
		avscore1 /= _size;
		avscore2 /= _size;

		TF.av1 = avscore1;
		TF.av2 = avscore2;
		TF.win1 = _size;
		TF.win2 = _size;

		for (int i = 0; i < _size; i+=2)
		{
			if (P1[i].avscore < P1[i + 1].avscore)
			{
				P1[i].killme = true;
				P1[i + 1].killme = false;
			}
			else
			{
				P1[i].killme = false;
				P1[i + 1].killme = true;
			}
			if (P2[i].avscore < P2[i + 1].avscore)
			{
				P2[i].killme = true;
				P2[i + 1].killme = false;
			}
			else
			{
				P2[i].killme = false;
				P2[i + 1].killme = true;
			}
		}
		/*for (int i = 0; i < _size; i++)
		{
			if (P1[i].avscore < avscore1)P1[i].killme = true;
			else P1[i].killme = false;
			if (P2[i].avscore < avscore2)P2[i].killme = true;
			else P2[i].killme = false;
		}*/


		for (int i = 0; i < _size; i++)
		{
			if (P1[i].killme)
			{
				/*if (P1[i].avscore < 12)
				{
					P1[i] = NW(LS, 9);
					continue;
				}*/
				int l = i - 1;
				int r = i + 1;
				if (l < 0)l = _size - 1;
				if (r > _size - 1)r = 0;
				while (P1[l].killme)
				{
					l--;
					if (l < 0)l = _size - 1;
				}
				while (P1[r].killme)
				{
					r++;
					if (r > _size - 1)r = 0;
				}
				P1[i].MakeChild(P1[l], P1[r]);
			}
			if (P2[i].killme)
			{
				/*if (P2[i].avscore < 7)
				{
					P2[i] = NW(LS, 9);
					continue;
				}*/
				int l = i - 1;
				int r = i + 1;
				if (l < 0)l = _size - 1;
				if (r > _size - 1)r = 0;
				while (P2[l].killme)
				{
					l--;
					if (l < 0)l = _size - 1;
				}
				while (P2[r].killme)
				{
					r++;
					if (r > _size - 1)r = 0;
				}
				P2[i].MakeChild(P2[l], P2[r]);
			}
		}

		/*for (int i = 0; i < _size; i++)
		{
			if (P1[i].killme)P1[i] = NW(LS, 9);
			if (P2[i].killme)P2[i] = NW(LS, 9);
		}*/
		
	
		for (int i = 0; i < _size; i++)
		{
			//var = MutationVaraity * (PERFECT_SCORE - P1[i].avscore) / PERFECT_SCORE;
			if (rand(0, 1) < MutationVaraity)P1[i].Mutate();

			//var = MutationVaraity * (PERFECT_SCORE - P2[i].avscore) / PERFECT_SCORE;
			if (rand(0, 1) < MutationVaraity)P2[i].Mutate();

		}

		

		SendMessageW(parent, MS_INCRBAR, NULL, (LPARAM) & TF);
		counter++;
		if (counter > TrainLimit)break;
		//if ((fabs(avscore1 - 18) < 0.1) && (fabs(avscore2 - 18) < 0.1))break;
		if (stop)break;

		GetBest(b1, b2);
		if (b1->avscore > 26)
		{
			b1found = true;
			b1final = new NW(*b1);
		}
		if (b2->avscore > 26)
		{
			b2found = true;
			b2final = new NW(*b2);
		}
		if (b1found && b2found)break;
	}
	gamelog.close();
}

void Trainer::GetBest(link& p1, link& p2)
{
	int maxp1 = 0;
	int maxp2 = 0;
	for (int i = 0; i < _size; i++)
	{
		if (P1[maxp1].avscore < P1[i].avscore)maxp1 = i;
		if (P2[maxp2].avscore < P2[i].avscore)maxp2 = i;
	}

	if (p1 != nullptr)delete p1;
	if (p2 != nullptr)delete p2;
	p1 = new NW(P1[maxp1]);
	p2 = new NW(P2[maxp2]);
}







int Max(vector<double>& p)
{
	int res = 0;
	bool move = true;
	if (p[0] != INT_MAX)move = false;
	for (int i = 1; i < p.size(); i++)
	{
		if (p[i] == INT_MAX)
		{
			if (move)res++;
			continue;
		}
		else
		{
			if (move)
			{
				res++;
				move = !move;
			}
		}
		if (p[res] < p[i])res = i;
	}
	return res;
}


