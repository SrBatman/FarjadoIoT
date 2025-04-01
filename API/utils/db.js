const { connect, set } = require('mongoose');
set('strictQuery', true);

const connectDB = async () => {
    try {
        await connect(process.env.MONGO_URI);
        console.log('Database connected');
    } catch (error) {
        console.log(error);
    }
};  

module.exports = { connectDB };
