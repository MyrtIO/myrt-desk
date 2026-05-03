import { viteSingleFile } from "vite-plugin-singlefile"
import { defineConfig } from "vite";

export default defineConfig({
    plugins: [viteSingleFile()],
    server: {
        proxy: {
            '/api': {
                target: 'http://192.168.4.1',
                changeOrigin: true,
                // rewrite: (path) => path.replace(/^\/api/, ''),
            },
        }
    },
    // build: {
    //     minify: 'terser',
    //     terserOptions: {
    //         mangle: {
    //             properties: true,
    //             toplevel: true,
    //         },
    //         module: true,
    //         toplevel: true,
    //     },
    // }
});
