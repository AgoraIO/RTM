module.exports = {
  plugins: [
    require('autoprefixer'),
    require('postcss-import'),
    require('postcss-nested'),
    require('postcss-preset-env')({
      features: {
        'custom-media-queries': true,
      },
    }),
    require('postcss-reporter'),
  ],
};
