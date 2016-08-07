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
		Texture2D tileSheet;

		SpriteBatch spriteBatch;
		public Render(Game1 game)
		{
			spriteBatch = new SpriteBatch(game.GraphicsDevice);
		}

		public void loadGraphics(ContentManager content)
		{
			tileSheet = content.Load<Texture2D>("visual/tilesheet");
		}

		public void renderGame()
		{
			spriteBatch.Begin();

			spriteBatch.Draw(tileSheet, new Rectangle(0, 0, 640, 480), Color.White);

			spriteBatch.End();
		}
	}
}
