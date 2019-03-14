const path = require('path');
const webpackMerge = require('webpack-merge');
const webpackConfigBase = require('./webpack.config.base.js');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const portfinder = require('portfinder');

const webpackConfigDev = webpackMerge(webpackConfigBase, {
  module: {
    rules: [
      {
        test: /\.css$/,
        use: [
          'style-loader', { loader: 'css-loader', options: { importLoaders: 1 } }, 'postcss-loader',
        ],
      },
    ],
  },
  devServer: {
    open: true,
    contentBase: path.resolve(__dirname, '../dist'),
    port: 8080,
    watchOptions: {
      poll: 1000,
    },
    stats: {
      children: false,
    },
  },
  plugins: [
    new MiniCssExtractPlugin({
      filename: 'css/[name].css',
      chunkFilename: 'css/[id].css',
    }),
  ],
});

module.exports = new Promise((resolve, reject) => {
  portfinder.basePort = 8080;
  portfinder.getPort((err, port) => {
    if (err) {
      reject(err);
    } else {
      webpackConfigDev.devServer.port = port;
      resolve(webpackConfigDev);
    }
  });
});
