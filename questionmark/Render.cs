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
	public class Render
	{
		SpriteBatch spriteBatch;

		public Vector2 camPos;
		public float zoom = 3;

		const int tileSize = 12;

		Texture2D tileSheet;

		GraphicsDevice device;

		public Render(Game1 game)
		{
			device = game.GraphicsDevice;
			spriteBatch = new SpriteBatch(game.GraphicsDevice);
		}

		public void loadGraphics(ContentManager content)
		{
			tileSheet = content.Load<Texture2D>("visual/tilesheet");
		}

		float m = 0;
		public void renderWorld(World world)
		{
			float scale = tileSize * zoom;
			camPos.X = m * scale;
			m += 0.01f;
			Matrix camMatrix = Matrix.CreateTranslation(new Vector3(-camPos, 0)) * Matrix.CreateTranslation(new Vector3(device.Viewport.Width / 2,
				device.Viewport.Height / 2, 0));
			// background
			spriteBatch.Begin(SpriteSortMode.Deferred, null, SamplerState.PointClamp, null, null, null, camMatrix);
			spriteBatch.Draw(tileSheet, new Vector2(0f * scale, 0f * scale), new Rectangle(12 * 12, 0, 12, 12), Color.White, 0, new Vector2(0, 0), zoom, SpriteEffects.None, 0);
			spriteBatch.Draw(tileSheet, new Vector2(1f * scale, 1f * scale), new Rectangle(12 * 14, 0, 12, 12), Color.White, 0, new Vector2(0, 0), zoom, SpriteEffects.None, 0);
			spriteBatch.End();
		}
	}
}
