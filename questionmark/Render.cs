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

		Texture2D tileSheet;

		public Render(Game1 game)
		{
			spriteBatch = new SpriteBatch(game.GraphicsDevice);
		}

		public void loadGraphics(ContentManager content)
		{
			tileSheet = content.Load<Texture2D>("visual/tilesheet");
		}

		public void renderWorld(World world)
		{
			// background
			spriteBatch.Begin(SpriteSortMode.Deferred, null, SamplerState.PointClamp, null, null, null, null);
			spriteBatch.Draw(tileSheet, new Rectangle(0, 0, 36, 36), new Rectangle(12 * 13, 0, 12, 12), Color.White);
			spriteBatch.End();
		}
	}
}
