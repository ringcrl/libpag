import { PAGGlUnit } from './pag-gl-unit';
import { PAGGlContext } from './pag-gl-context';

export class PAGGlFrameBuffer extends PAGGlUnit {
  public static create(gl: WebGLRenderingContext | number, framebuffer: WebGLFramebuffer | number) {
    let pagGlContext: PAGGlContext | null = null;
    if (typeof gl === 'number') {
      pagGlContext = new PAGGlContext(gl);
    }
    if (gl instanceof WebGLRenderingContext) {
      pagGlContext = PAGGlContext.get(gl);
    }
    if (!pagGlContext) throw new Error('Parameter error!');

    let id = -1;
    if (typeof framebuffer === 'number') {
      id = framebuffer;
    }
    if (framebuffer instanceof WebGLFramebuffer) {
      const { GL } = this.module;
      id = GL.getNewId(GL.framebuffers);
      GL.framebuffers[id] = framebuffer;
    }
    if (id < 0) throw new Error('Parameter error!');
    return new PAGGlFrameBuffer(pagGlContext, id);
  }

  public destroy(): void {
    PAGGlFrameBuffer.module.GL.framebuffers[this.id] = null;
  }
}
