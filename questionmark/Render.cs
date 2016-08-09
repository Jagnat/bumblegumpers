using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace questionmark
{
	public struct TileRenderInfo
	{
		public int xOffset, yOffset;
		public int numSubtiles;

		public TileRenderInfo(int xo, int yo, int nst)
		{
			xOffset = xo;
			yOffset = yo;
			numSubtiles = nst;
		}
	}

	public class Render
	{
		SpriteBatch spriteBatch;

		Matrix camMatrix;
		public float zoom = 4;

		const int tilePixels = 12;

		TileRenderInfo[] tileInfo;
		Rectangle playerRect;

		Texture2D tileSheet;

		GraphicsDevice device;

		public Render(Game1 game)
		{
			device = game.GraphicsDevice;
			spriteBatch = new SpriteBatch(game.GraphicsDevice);
			setCamPos(new Vector2(8, 6));

			playerRect = new Rectangle(0, 31 * 12, 12, 12);

			setupTileRegions();
		}

		private void setupTileRegions()
		{
			tileInfo = new TileRenderInfo[16];
			tileInfo[0] = new TileRenderInfo(0, 0, 0);
			tileInfo[1] = new TileRenderInfo(0, 0, 13);
			tileInfo[2] = new TileRenderInfo(13, 0, 11);
			tileInfo[3] = new TileRenderInfo(24, 0, 8);
			tileInfo[4] = new TileRenderInfo(0, 1, 12);
			tileInfo[5] = new TileRenderInfo(12, 1, 9);
		}

		public void loadGraphics(ContentManager content)
		{
			tileSheet = content.Load<Texture2D>("visual/tilesheet");
		}

		public void setCamPos(Vector2 pos)
		{
			camMatrix = Matrix.CreateTranslation(new Vector3(-pos * tilePixels * zoom, 0)) * Matrix.CreateTranslation(new Vector3(device.Viewport.Width / 2,
				device.Viewport.Height / 2, 0));
		}

		public void renderWorld(World world)
		{
			float scale = tilePixels * zoom;

			spriteBatch.Begin(SpriteSortMode.BackToFront, null, SamplerState.PointClamp, null, null, null, camMatrix);
			for (int x = 0; x < world.width; ++x)
			{
				for (int y = 0; y < world.height; ++y)
				{
					Tile current = world.tiles[x, y];
					if (current.backgroundTile != TileType.EMPTY)
					{
						TileRenderInfo info = tileInfo[(int)current.backgroundTile];
						Rectangle src = new Rectangle((info.xOffset + current.backgroundIndex) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
						spriteBatch.Draw(tileSheet, new Vector2(x * scale, y * scale), src, Color.White, 0, Vector2.Zero, zoom, SpriteEffects.None, 1);
					}
					if (current.mainTile != TileType.EMPTY)
					{
						TileRenderInfo info = tileInfo[(int)current.mainTile];
						Rectangle src = new Rectangle((info.xOffset + current.mainIndex) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
						spriteBatch.Draw(tileSheet, new Vector2(x * scale, y * scale), src, Color.White, 0, Vector2.Zero, zoom, SpriteEffects.None, .5f);
					}
					if (current.foregroundTile != TileType.EMPTY)
					{
						TileRenderInfo info = tileInfo[(int)current.foregroundTile];
						Rectangle src = new Rectangle((info.xOffset + current.foregroundIndex) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
						spriteBatch.Draw(tileSheet, new Vector2(x * scale, y * scale), src, Color.White, 0, Vector2.Zero, zoom, SpriteEffects.None, 0);
					}
				}
			}
			spriteBatch.End();
		}

		public void renderPlayer(Player player)
		{
			float scale = tilePixels * zoom;
			spriteBatch.Begin(SpriteSortMode.BackToFront, null, SamplerState.PointClamp, null, null, null, camMatrix);
			spriteBatch.Draw(tileSheet, new Vector2(player.X * scale, player.Y * scale), playerRect, Color.White, 0, Vector2.Zero, zoom, SpriteEffects.None, .5f);
			spriteBatch.End();
		}
	}
}
