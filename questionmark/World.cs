using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace questionmark
{
	public enum TileType : ushort
	{
		EMPTY = 0,
		DIRT = 1,
		BRICK = 2,
		WOOD = 3,
		SQUARE = 4
	}

	[Serializable]
	public struct Tile
	{
		public bool collidable;
		public TileType backgroundTile, mainTile, foregroundTile;
		public byte backgroundIndex, mainIndex, foregroundIndex;
	}

	public class World
	{
		public int width, height;
		public Tile[,] tiles;

		public World(int w, int h)
		{
			width = w;
			height = h;
			tiles = new Tile[width, height];

			initTestWorld();
		}

		public void initTestWorld()
		{
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					tiles[x, y].backgroundTile = TileType.BRICK;
					tiles[x, y].backgroundIndex = 4;
				}
			}
		}
	}
}
