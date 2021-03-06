#include "graphics.h"


sf::Color getColor(unsigned int val, unsigned int max)
{
	if(val >= max){
		return sf::Color(255,0,0);
	}
	else if(val <= max/2)
	{
		return sf::Color(0, (255/(max/2))*val, 255-(255/(max/2)*val)); 
	}

	return sf::Color((255/(max/2)*(val-max/2)),255 - (255/(max/2)*(val-max/2)), 0); 
}

void updateDoses(vector< vector< Voxel > > & voxels, vector< BeamLet> & top, vector< BeamLet > & right)
{
	for(unsigned int i=0;i<right.size();++i){
		for(unsigned int j=0;j<top.size();++j){
			voxels[i][j].setDose(right[i].strength+top[j].strength);
		}
	}
}

void showTypes(vector<vector<Voxel> >& voxels)
{
	for(unsigned int i=0;i<voxels.size();++i)
	{
		for(unsigned int j=0;j<voxels[i].size();++j)
		{
			if(voxels[i][j].type == PTV){
				voxels[i][j].setFillColor(sf::Color::Red);
			}else if(voxels[i][j].type == OAR){
				voxels[i][j].setFillColor(sf::Color::Blue);
			}else{
				voxels[i][j].setFillColor(sf::Color::Black);
			}
		}
	}
}

vector< vector< Voxel> > initVoxels(sf::RenderWindow & window, unsigned rows, unsigned cols,
   unsigned vox_size)
{
	vector< vector<Voxel> > voxels( rows, vector<Voxel>(cols) );
	int x_start, y_start;
	x_start = (int)(window.getView().getSize().x/2 - (rows/2)*vox_size + vox_size/2);
	y_start = (int)(window.getView().getSize().y/2 -(cols/2)*vox_size + vox_size/2);

	for(unsigned int i=0;i<rows;++i)
	{
		for(unsigned int j=0;j<cols;++j)
		{
			// position voxels
			voxels[i][j].setSize(sf::Vector2f(vox_size, vox_size));
			voxels[i][j].setOrigin(voxels[i][j].getGlobalBounds().width/2.f,
				voxels[i][j].getGlobalBounds().height/2.f);
			voxels[i][j].setPosition(float(x_start + j*vox_size), float(y_start + i*vox_size));
			voxels[i][j].setFillColor(sf::Color::Transparent);
			voxels[i][j].setOutlineThickness(2.f);
			voxels[i][j].setOutlineColor(sf::Color::Cyan);
		}
	}
	return voxels;
}

vector< BeamLet > initTop(sf::RenderWindow & window, unsigned rows, unsigned cols,
   unsigned vox_size)
{
	int x_start, y_start;
	x_start = (int)(window.getView().getSize().x/2 - (rows/2)*vox_size + vox_size/2);
	y_start = (int)(window.getView().getSize().y/2 -(cols/2)*vox_size + vox_size/2);
	
	vector< BeamLet > top(cols);
	// positon beamlets
	for(unsigned int i=0;i<rows;++i)
	{
		top[i].vert = false;
		top[i].strength = i;
		top[i].setSize(sf::Vector2f(float(vox_size)*.75f, float(top[i].strength)));
		top[i].setOrigin(top[i].getGlobalBounds().width/2, 0.f);
		top[i].setPosition(float(x_start + i*vox_size), float(y_start - 2*vox_size));
		top[i].setFillColor(sf::Color::Magenta);
	}

	return top;
}

vector< BeamLet > initRight(sf::RenderWindow & window, unsigned rows, unsigned cols,
   unsigned vox_size)
{
	int x_start, y_start;
	x_start = (int)(window.getView().getSize().x/2 - (rows/2)*vox_size + vox_size/2);
	y_start = (int)(window.getView().getSize().y/2 -(cols/2)*vox_size + vox_size/2);

	vector< BeamLet > right(rows);
	for(unsigned int i=0;i<cols;++i)
	{
		right[i].vert = true;
		right[i].strength = i;
		right[i].setSize(sf::Vector2f(float(right[i].strength), float(vox_size)*.75f));
		right[i].setOrigin(0.f, right[i].getGlobalBounds().height/2);
		right[i].setPosition(float(x_start + cols*vox_size + vox_size), float(y_start + i*vox_size));
		right[i].setFillColor(sf::Color::Magenta);
	}

	return right;
}

void handleEvents(sf::RenderWindow & Window, unsigned int & cur_mode, bool & solved,
	vector<vector<Voxel> >& voxels, vector< BeamLet >& top, vector< BeamLet>& right)
{
	sf::Event event;
	while(Window.pollEvent(event))
	{
		if(event.type == sf::Event::Closed){
			Window.close();
		}

		if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)){
			Window.close();
		}

      // key contral event
		if(event.type == sf::Event::KeyPressed)
      {
         if(event.key.code == sf::Keyboard::Return)
         {
             if(cur_mode == SET_TYPE && !solved){
                cur_mode = SHOW_DOSES;
                // calc new doses
                updateDoses(voxels, top, right);

             }else if(cur_mode == SHOW_DOSES){
                cur_mode = SOLVE;
             }
             else if(cur_mode == SOLVE && solved)
             {
                cur_mode = SHOW_TYPE;
                showTypes(voxels);
             }
             else if(cur_mode == SHOW_TYPE && solved)
             {
                cur_mode = SHOW_DOSES;
                updateDoses(voxels,top,right);
             }
         }
		}

		if(cur_mode == SET_TYPE){
			if(event.type == sf::Event::MouseButtonPressed && // set PTV
				event.mouseButton.button == sf::Mouse::Left)
			{
				int x, y;
				x = event.mouseButton.x; y = event.mouseButton.y;

				sf::Vector2f convert = Window.convertCoords(sf::Vector2i(x, y));
				x = int(convert.x);
				y = int(convert.y);

				for(unsigned int i=0;i<voxels.size();++i){
					for(unsigned int j=0;j<voxels[i].size();++j){
						if(voxels[i][j].getGlobalBounds().contains(float(x),float(y))){
							voxels[i][j].setFillColor(sf::Color::Red);
							voxels[i][j].type = PTV;
						}
					}
				}
			}

			if(event.type == sf::Event::MouseButtonPressed && // set OAR
				event.mouseButton.button == sf::Mouse::Right)
			{
				int x, y;
				x = event.mouseButton.x; y = event.mouseButton.y;

				sf::Vector2f convert = Window.convertCoords(sf::Vector2i(x, y));
				x = int(convert.x);
				y = int(convert.y);

				for(unsigned int i=0;i<voxels.size();++i){
					for(unsigned int j=0;j<voxels[i].size();++j){
						if(voxels[i][j].getGlobalBounds().contains(float(x),float(y))){
							voxels[i][j].setFillColor(sf::Color::Blue);
							voxels[i][j].type = OAR;
						}
					}
				}
			}
		}// end if(cur_mode == SET_TYPE)
	}
}

void display(sf::RenderWindow& Window, vector<vector<Voxel> >&voxels, vector<BeamLet>& right,
	vector<BeamLet>& top)
{
	// draw voxels
	for(unsigned int i=0;i<voxels.size();++i){
		for(unsigned int j=0;j<voxels[i].size();++j){
			Window.draw(voxels[i][j]);
		}
	}
	// draw beamlets
	for(unsigned int i=0;i<right.size();++i){
		Window.draw(right[i]);
	}
	for(unsigned int i=0;i<top.size();++i){
		Window.draw(top[i]);
	}
}

vector< vector<IloInt> > getVoxMat(vector< vector< Voxel> >& voxels,
	vector<BeamLet>& top, vector<BeamLet>& right, IloInt& m, IloInt& n)
{
	n = right.size() + top.size();
	m = voxels.size()*voxels[0].size();

	// does voxel i gets from beamlet j
	vector< vector<IloInt> > vox(m, n);
	for(unsigned int i=0;i<voxels.size();++i)
	{
		for(unsigned int j=0;j<voxels[0].size();++j)
		{
			for(int it=0;it<n;++it)
			{
				if(it < (int)(top.size()))
				{
					if(j == it){
						vox[i*top.size()+j][it] = 1;
					}else{
						vox[i*top.size()+j][it] = 0;
					}
				}
				else
				{
					if(i == (it - top.size())){
						vox[i*top.size()+j][it] = 1;
					}else{
						vox[i*top.size()+j][it] = 0;
					}
				}
			}
		}
	}

	return vox;
}

void setPTVandOAR(vector<vector<Voxel> >& voxels, Set & X, Set & Y)
{
	for(unsigned int i=0;i<voxels.size();++i)
	{
		for(unsigned int j=0;j<voxels[0].size();++j)
		{
			if(voxels[i][j].type == PTV){
				Y.push_back(i*unsigned(voxels[i].size())+j);
			}else if(voxels[i][j].type == OAR){
				X.push_back(i*unsigned(voxels[i].size())+j);
			}
		}
	}
}

void createPTVandOAR(vector<vector<Voxel> > & voxels, Set & X, Set & Y)
{
   unsigned m, n;
   m = voxels.size();
   n = voxels[0].size();
   for(unsigned i=0;i<X.size();++i)
   {
      unsigned col = X[i];
      unsigned row=0;
      while(col >= n)
      {
         col-=n;
         ++row;
      }

      voxels[row][col].setFillColor(sf::Color::Blue);
      voxels[row][col].type = OAR;
   }

   for(unsigned i=0;i<Y.size();++i)
   {
      unsigned col = Y[i];
      unsigned row=0;
      while(col >= n)
      {
         col-=n;
         ++row;
      }

      voxels[row][col].setFillColor(sf::Color::Red);
      voxels[row][col].type = PTV;
   }
}

void setDosesFromSolution(IloNumArray& vals, vector<BeamLet>& top, vector<BeamLet>& right)
{
	for(unsigned int i=0;i<top.size();++i)
	{
		top[i].setStrength(unsigned int(vals[i]));
	}
	for(unsigned int i=unsigned(top.size());i<unsigned(top.size()+right.size());++i)
	{
		right[i-top.size()].setStrength(unsigned int(vals[i]));
	}
}