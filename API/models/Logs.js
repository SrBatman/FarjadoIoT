const { Schema, model } = require('mongoose');

const logSchema = new Schema({
    username: {
      type: String,
      required: true,
      default: 'Desconocido',
    },
    sensor: {
      type: String,
      required: true,
    },
    type: {
      type: String,
      required: true,
    },
    endedDate: {
      type: Number,
      required: false,
      default: null,
    },
  }, {
    timestamps: true,
  });

module.exports = model('log', logSchema);