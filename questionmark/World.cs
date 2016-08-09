﻿using System;
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
		SQUARE = 4,
		GRASS = 5
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

		public void setBackgroundTile(int x, int y, TileType type, byte index)
		{
			tiles[x, y].backgroundTile = type;
			tiles[x, y].backgroundIndex = index;
		}

		public void setMainTile(int x, int y, TileType type, byte index)
		{
			tiles[x, y].mainTile = type;
			tiles[x, y].mainIndex = index;
		}

		public void setForegroundTile(int x, int y, TileType type, byte index)
		{
			tiles[x, y].foregroundTile = type;
			tiles[x, y].foregroundIndex = index;
		}

		public void initTestWorld()
		{
			for (int x = 0; x < width; x++)
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

			setMainTile(11, 8, TileType.DIRT, 9);
			setMainTile(12, 8, TileType.DIRT, 3);
			setMainTile(13, 8, TileType.DIRT, 6);

			setForegroundTile(11, 7, TileType.GRASS, 1);
			setForegroundTile(12, 7, TileType.GRASS, 4);
			setForegroundTile(13, 7, TileType.GRASS, 5);

			setForegroundTile(11, 9, TileType.GRASS, 6);
			setForegroundTile(12, 9, TileType.GRASS, 8);
			setForegroundTile(13, 9, TileType.GRASS, 7);
		}
	}
}
