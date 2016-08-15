using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bumblegumpers
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
		Vector2 camPos;
		public float zoom = 4;

		const int tilePixels = 12;

		TileRenderInfo[] tileInfo;
		Rectangle playerRect;

		Texture2D tileSheet;
		Texture2D whiteTex;
		SpriteFont font;

		public GraphicsDevice device;

		public Render(BumbleGumpers game)
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
			tileInfo[6] = new TileRenderInfo(21, 1, 4);
		}

		public void loadGraphics(ContentManager content)
		{
			tileSheet = content.Load<Texture2D>("visual/tilesheet");
			font = content.Load<SpriteFont>("questrial");
			whiteTex = new Texture2D(device, 1, 1);
			whiteTex.SetData(new[] { Color.White });
		}

		public void refreshCamera()
		{
			camMatrix = Matrix.CreateTranslation(new Vector3(-camPos * tilePixels * zoom, 0)) * Matrix.CreateTranslation(new Vector3(device.Viewport.Width / 2,
				device.Viewport.Height / 2, 0));
		}

		public void setCamPos(Vector2 pos)
		{
			camPos = pos;
			refreshCamera();
		}

		public void startRender()
		{
			spriteBatch.Begin(SpriteSortMode.BackToFront, BlendState.NonPremultiplied, SamplerState.PointClamp, null, null, null, camMatrix);
		}

		public void endRender()
		{
			spriteBatch.End();
		}

		public void renderWorld(World world, float bOpacity = 1, float mOpacity = 1, float fOpacity = 1)
		{
			float scale = tilePixels * zoom;

			for (int x = 0; x < world.width; ++x)
			{
				for (int y = 0; y < world.height; ++y)
				{
					Tile current = world.tiles[x, y];
					if (current.backgroundTile != TileType.EMPTY)
					{
						TileRenderInfo info = tileInfo[(int)current.backgroundTile];
						Rectangle src = new Rectangle((info.xOffset + current.backgroundIndex) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
						spriteBatch.Draw(tileSheet, new Vector2(x * scale, y * scale), src, new Color(Color.White, bOpacity), 0, Vector2.Zero, zoom, SpriteEffects.None, 0.6f);
					}
					if (current.mainTile != TileType.EMPTY)
					{
						TileRenderInfo info = tileInfo[(int)current.mainTile];
						Rectangle src = new Rectangle((info.xOffset + current.mainIndex) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
						spriteBatch.Draw(tileSheet, new Vector2(x * scale, y * scale), src, new Color(Color.White, mOpacity), 0, Vector2.Zero, zoom, SpriteEffects.None, 0.5f);
					}
					if (current.foregroundTile != TileType.EMPTY)
					{
						TileRenderInfo info = tileInfo[(int)current.foregroundTile];
						Rectangle src = new Rectangle((info.xOffset + current.foregroundIndex) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
						spriteBatch.Draw(tileSheet, new Vector2(x * scale, y * scale), src, new Color(Color.White, fOpacity), 0, Vector2.Zero, zoom, SpriteEffects.None, 0.4f);
					}
				}
			}
		}

		public void renderPlayer(Player player)
		{
			float scale = tilePixels * zoom;
			spriteBatch.Draw(tileSheet, new Vector2(player.X * scale, player.Y * scale), playerRect, Color.White, 0, Vector2.Zero, zoom, SpriteEffects.None, .5f);
		}

		// DEBUG RENDERING FUNCTIONS

		public void startDebugRender()
		{
			spriteBatch.Begin(SpriteSortMode.BackToFront, null, SamplerState.PointClamp, null, null, null, null);
		}

		public void endDebugRender()
		{
			spriteBatch.End();
		}

		public void renderDebugRect(Rectangle rect, Color color, float depth)
		{
			//spriteBatch
			Rectangle? cR = new Rectangle(0, 0, 1, 1);
			spriteBatch.Draw(whiteTex, rect, cR, color, 0, Vector2.Zero, SpriteEffects.None, depth);
		}

		public void renderDebugText(int x, int y, Color c, float depth, String s, bool centerX = false, bool centerY = false)
		{
			Vector2 b = font.MeasureString(s);
			Vector2 p = new Vector2(centerX ? x - b.X / 2 : x, centerY ? y - b.Y / 2 : y);
			spriteBatch.DrawString(font, new StringBuilder(s), p, c, 0, Vector2.Zero, 1, SpriteEffects.None, depth);
		}

		public void renderDebugTile(Rectangle dest, TileType type, byte index, float depth)
		{
			TileRenderInfo info = tileInfo[(int)type];
			Rectangle src = new Rectangle((info.xOffset + index) * tilePixels, info.yOffset * tilePixels, tilePixels, tilePixels);
			renderDebugRect(dest, Color.Black, depth + 0.01f);
			spriteBatch.Draw(tileSheet, dest, src, Color.White, 0, Vector2.Zero, SpriteEffects.None, depth);
		}
	}
}
