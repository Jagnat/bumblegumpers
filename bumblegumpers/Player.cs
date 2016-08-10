using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bumblegumpers
{
	public class Player
	{
		public int X, Y;

		bool onGround;
		bool canMove;

		public Player(int x, int y)
		{
			X = x;
			Y = y;
		}

		public void update(InputState input, World world)
		{
			if (world.collides(X, Y + 1))
				onGround = true;
			else
				onGround = false;

			if (onGround)
			{
				// Left or right or jump
				canMove = true;
				if (input.upPressed && !world.collides(X, Y - 1))
					Y--;
				else if (input.leftPressed && !input.rightPressed && !world.collides(X - 1, Y))
					X--;
				else if (input.rightPressed && !input.leftPressed && !world.collides(X + 1, Y))
					X++;
			}
			else if (canMove)
			{
				// Left or right movement
				if (input.upPressed)
				{
					canMove = false;
				}
				else if (input.leftPressed && !input.rightPressed && !world.collides(X - 1, Y))
				{
					X--;
					canMove = false;
				}
				else if (input.rightPressed && !input.leftPressed && !world.collides(X + 1, Y))
				{
					X++;
					canMove = false;
				}
			}
			else
			{
				// Move player to ground
				int index = 0;
				while (!world.collides(X, Y + index))
					index++;
				Y += index - 1;
			}
		}
	}
}
