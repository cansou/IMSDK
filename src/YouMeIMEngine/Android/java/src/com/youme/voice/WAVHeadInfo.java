package com.youme.voice;

import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import com.youme.im.CommonConst;
import com.youme.im.IMEngine;

import android.util.Log;

public class WAVHeadInfo{
	// RIFF chunk
	private byte riffID[] = {'R', 'I', 'F', 'F'};					//"RIFF"
	private int fileSize = 0;
	private byte riffFormat[] = {'W', 'A', 'V', 'E'};				//"WAVE"
	// format chunk
	private byte fmtID[] = {'f', 'm', 't', 0X20};					//"fmt "
	private int fmtSize = 16;
	private short formatTag = 0x0001;								//编码方式
	private short channels = CommonConst.CHANNEL_NUMBER;			//声道数
	private int sampleRate = CommonConst.SAMPLERATE_16K;			//采样频率
	private int byteRate = 0;										//比特率
	private short blockAlign = 0;									//块对齐大小
	private short sampleBitSize = CommonConst.SAMPLE_BIT_SIZE;		//样本宽度
	// data chunk
	private byte dataID[] = {'d', 'a', 't', 'a'};					//"data"
	private int dataChunkSize = 0;
	
	public void SetAudioProperty(int sampleFrequency, int channel, int sampleBitSize){
		this.channels = (short) channel;
		this.sampleRate = sampleFrequency;
		this.sampleBitSize = (short) sampleBitSize;
		this.byteRate = sampleFrequency * channels * sampleBitSize / 8;
		this.blockAlign = (short) (channel * sampleBitSize / 8);
	}
	
	public boolean WriteHeadInfo(String recordPath){
		try{
			RandomAccessFile raf = new RandomAccessFile(recordPath, "rwd");
			int length = (int) raf.length();
			this.fileSize = length - 8;
			this.dataChunkSize = length - 44;
			
			ByteBuffer byteBuf = ByteBuffer.allocate(44);
			//if (ByteOrder.nativeOrder() != ByteOrder.LITTLE_ENDIAN){
				byteBuf.order(ByteOrder.LITTLE_ENDIAN);
			//}
			byteBuf.put(riffID);
			byteBuf.putInt(fileSize);
			byteBuf.put(riffFormat);
			byteBuf.put(fmtID);
			byteBuf.putInt(fmtSize);
			byteBuf.putShort(formatTag);
			byteBuf.putShort(channels);
			byteBuf.putInt(sampleRate);
			byteBuf.putInt(byteRate);
			byteBuf.putShort(blockAlign);
			byteBuf.putShort(sampleBitSize);
			byteBuf.put(dataID);
			byteBuf.putInt(dataChunkSize);
			raf.write(byteBuf.array(), 0, 44);
			raf.close();
		}catch(Exception e){
			IMEngine.WriteLog(CommonConst.LogLevel.LOG_LEVEL_ERROR, "write wav head failed");
			e.printStackTrace();
			return false;
		}
		
		return true;
	}
}