using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bumblegumpers
{
	public enum PlayerState
	{
		ONGROUND,
		AIRWALKING,
		JUMPING,
		CLIMBING,
		FALLING
	}

	public class Player
	{
		public int X, Y;
		PlayerState state;

		// Fully mobile

		public Player(int x, int y)
		{
			X = x;
			Y = y;
		}

		public void updateState(World world)
		{
			TileCollision under = world.collision[X, Y + 1];
			if (under == TileCollision.SOLID ||	under == TileCollision.LADDER)
				state = PlayerState.ONGROUND;
			if (world.collision[X, Y] == TileCollision.LADDER)
				state = PlayerState.CLIMBING;
		}

		private bool doLeftRight(InputState input, World world)
		{
			if (input.leftPressed && !input.rightPressed && world.col(X - 1, Y) != TileCollision.SOLID)
			{
				X--;
				return true;
			}
			else if (input.rightPressed && !input.leftPressed && world.col(X + 1, Y) != TileCollision.SOLID)
			{
				X++;
				return true;
			}
			return false;
		}

		private bool doUp(InputState input, World world)
		{
			if (input.upPressed && world.col(X, Y - 1) != TileCollision.SOLID)
			{
				Y--;
				return true;
			}
			return false;
		}

		private void fall(World world)
		{
			while (true)
			{
				TileCollision at = world.collision[X, Y];
				TileCollision under = world.collision[X, Y + 1];
				if (under == TileCollision.LADDER || under == TileCollision.SOLID || at == TileCollision.LADDER)
					break;
				Y++;
			}
		}

		public void update(InputState input, World world)
		{
			switch (state)
			{
				case PlayerState.ONGROUND:
					{
						if (input.downPressed &&
							world.col(X, Y + 1) == TileCollision.LADDER)
							Y++;
						if (doUp(input, world))
							state = PlayerState.JUMPING;
						else
						{
							doLeftRight(input, world);
							state = PlayerState.AIRWALKING;
						}
						updateState(world);
					}
					break;
				case PlayerState.AIRWALKING:
					{
						if (input.downPressed)
							fall(world);
						else if (doLeftRight(input, world))
							fall(world);
						updateState(world);
					}
					break;
				case PlayerState.JUMPING:
					{
						if (input.downPressed)
							fall(world);
						else if (doUp(input, world))
						{
							fall(world);
						}
						else if (doLeftRight(input, world))
							fall(world);
						updateState(world);
					}
					break;
				case PlayerState.CLIMBING:
					{
						if (input.downPressed)
						{
							if (world.col(X, Y + 1) != TileCollision.SOLID)
								Y++;
							fall(world);
						}
						else if (doUp(input, world))
							fall(world);
						else if (doLeftRight(input, world))
							state = PlayerState.AIRWALKING;
						updateState(world);

					}
					break;
				default:
					break;
			}
		}
	}
}
