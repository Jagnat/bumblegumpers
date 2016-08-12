using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bumblegumpers
{
	public class Editor
	{
		public Editor()
		{

		}

		public void updateEditor(InputState input)
		{
			
		}

		public void renderEditor(Render renderer)
		{
			renderer.startDebugRender();
			renderer.renderDebugText(0, 0, Color.Black, "TEST");
			renderer.renderDebugRect(new Rectangle(0, 0, 100, 100), new Color(Color.Red, 0.5f));
			renderer.endDebugRender();
		}
	}
}
