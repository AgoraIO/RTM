const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const { getEntries } = require('./utils.js');
const webpack = require('webpack');
const {resolve} = path;

const entries = getEntries('./src/pages/', 'js');

const config = {
  entry: Object.assign(entries, {
    app: './src/app.js',
    'rtm.min': './src/assets/rtm.min.js'
  }),
  output: {
    pathinfo: false,
    path: path.resolve(__dirname, '../dist'),
    filename: 'js/[name].[hash:8].js',
    chunkFilename: 'js/[name].chunk.[chunkhash:8].js',
    // publicPath: '/',
  },
  resolve: {
    alias: {
      src: path.resolve(__dirname, '../src'),
      components: path.resolve(__dirname, '../src/components'),
    },
    alias: {
      rtm: resolve(__dirname, "../src/assets/rtm.min.js"),
    }
  },
  module: {
    rules: [
      {
        test: /\.js$/,
        exclude: /node_modules/,
        use: [{
          loader: 'babel-loader',
          options: {
            root: '../'
          }
        }],
      },
      {
        test: /\.html$/,
        use: [
          {
            loader: 'html-loader',
            options: {
              minimize: true,
            },
          },
        ],
      },
      {
        test: /\.(png|jpg|gif)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
        use: [
          {
            loader: 'url-loader',
            options: {
              limit: 10000,
              name: 'images/[name].[md5:hash:hex:8].[ext]',
            },
          },
        ],
      },
      {
        test: /\.(woff|woff2|otf|ttf|eot)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
        use: [
          {
            loader: 'url-loader',
            options: {
              limit: 10000,
              name: 'fonts/[name].[md5:hash:hex:8].[ext]',
            },
          },
        ],
      },
      {
        test: /\.(mp4|ogg|svg)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
        use: [
          {
            loader: 'file-loader',
            options: {
              name: 'assets/[name].[md5:hash:hex:8].[ext]',
            },
          },
        ],
      },
    ],
  },
  parallelism: 8,
  optimization: {
    splitChunks: {
      chunks: 'all',
      cacheGroups: {
        commons: {
          name: 'commons',
          chunks: 'initial',
          minChunks: 2,
        },
        vendors: {
          chunks: 'initial',
          name: 'vendors',
          test: /node_modules\//,
          minChunks: 5,
          priority: 10,
        },
        default: {
          minChunks: 2,
          priority: -20,
          reuseExistingChunk: true,
        },
      },
    },
  },
  plugins: [
    new webpack.ProvidePlugin({
      $: 'jquery',
      jQuery: 'jquery',
      AgoraRTM: "rtm"
    }),
  ],
};

const pages = getEntries('./src/pages/', 'html');

for (const pathname in pages) {
  // Configured to generate the html file, define paths, etc.
  const conf = {
    filename: `${pathname}.html`, // html output pathname
    template: path.resolve(__dirname, `.${pages[pathname]}`), // Template path
    inject: true,
    favicon: path.resolve(__dirname, '../src/assets/favicon.ico'),
    chunks: ['commons', 'vendors', 'app', pathname],
    chunksSortMode: 'manual',
  };
  config.plugins.push(new HtmlWebpackPlugin(conf));
}

module.exports = config;
