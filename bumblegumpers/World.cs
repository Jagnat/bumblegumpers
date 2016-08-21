using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bumblegumpers
{
	public enum TileCollision : byte
	{
		EMPTY = 0,
		SOLID = 1,
		LADDER = 2,
		CORNER = 3,
		PLATFORM = 4
	}

	public class TileType
	{
		static Dictionary<ushort, TileType> tileGroups;
		private static int numIds = 0;
		string name;
		int num;
		Rectangle[] regions;

		public TileType(string name, int num)
		{
			this.name = name;
			// construct regions;
		}

		public static void initTileInfo()
		{
			tileGroups = new Dictionary<ushort, TileType>();
		}
	}

	public class World
	{
		public int width, height;
		public ushort[,] bTiles;
		public ushort[,] mTiles;
		public ushort[,] fTiles;
		public TileCollision[,] collision;

		public World(int w, int h)
		{
			width = w;
			height = h;
			bTiles = new ushort[width, height];
			mTiles = new ushort[width, height];
			fTiles = new ushort[width, height];
			collision = new TileCollision[width, height];

			initTestWorld();
		}

		public TileCollision col(int x, int y)
		{
			return collision[x, y];
		}

		public void setBackgroundTile(int x, int y, ushort tile)
		{
			bTiles[x, y] = tile;
		}

		public void setMainTile(int x, int y, ushort tile)
		{
			mTiles[x, y] = tile;
		}

		public void setForegroundTile(int x, int y, ushort tile)
		{
			fTiles[x, y] = tile;
		}

		public void initTestWorld()
		{
			/*for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					setBackgroundTile(x, y, TileType.BRICK, 4);
					setMainTile(0, y, TileType.WOOD, 1);
					setMainTile(x, 0, TileType.WOOD, 1);
					setMainTile(width - 1, y, TileType.WOOD, 1);
					setMainTile(x, height-1, TileType.WOOD, 1);
				}
			}
			setMainTile(3, 10, TileType.BRICK, 0);

			setMainTile(7, 10, TileType.SQUARE, 0);
			setMainTile(8, 10, TileType.SQUARE, 1);
			setMainTile(8, 9, TileType.SQUARE, 0);
			setMainTile(9, 10, TileType.SQUARE, 1);
			setMainTile(9, 9, TileType.SQUARE, 1);
			setMainTile(9, 8, TileType.SQUARE, 0);

			setBackgroundTile(9, 5, TileType.LADDER, 0);
			setBackgroundTile(9, 4, TileType.LADDER, 2);

			setMainTile(8, 4, TileType.WOOD, 4);
			setMainTile(7, 4, TileType.WOOD, 4);
			setMainTile(6, 4, TileType.WOOD, 4);

			setMainTile(11, 8, TileType.DIRT, 9);
			setMainTile(12, 8, TileType.DIRT, 3);
			setMainTile(13, 8, TileType.DIRT, 6);

			setForegroundTile(11, 7, TileType.GRASS, 1);
			setForegroundTile(12, 7, TileType.GRASS, 4);
			setForegroundTile(13, 7, TileType.GRASS, 5);

			setForegroundTile(11, 9, TileType.GRASS, 6);
			setForegroundTile(12, 9, TileType.GRASS, 8);
			setForegroundTile(13, 9, TileType.GRASS, 7);

			setBackgroundTile(14, 10, TileType.LADDER, 0);
			setBackgroundTile(14, 9, TileType.LADDER, 0);
			setBackgroundTile(14, 8, TileType.LADDER, 2);*/
		}
	}
}
