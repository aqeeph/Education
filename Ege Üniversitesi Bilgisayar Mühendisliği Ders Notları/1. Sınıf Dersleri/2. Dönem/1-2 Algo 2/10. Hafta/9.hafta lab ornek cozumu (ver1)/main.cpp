#include <iostream>

using std::cout;
using std::endl;

class Karmasik {
private:
    double reel;
    double sanal;
public:
    Karmasik(double, double);
    void topla(double, double);
    void cikar(double, double);
    void goster();
};

Karmasik::Karmasik(double r, double s)
{
    reel=r;
    sanal=s;
}

void Karmasik::topla(double r, double s)
{
    reel=reel+r;
    sanal=sanal+s;
}

void Karmasik::cikar(double r, double s)
{
    reel=reel-r;
    sanal=sanal-s;
}

void Karmasik::goster()
{
    cout << '(' << reel << ", " << sanal << ')' << endl;
}

int main()
{
	Karmasik k1(2,3);

	k1.goster();
	k1.topla(1,1);
	k1.goster();
	k1.cikar(1,1);
	k1.goster();

	cout << endl;
	return 0;
}
