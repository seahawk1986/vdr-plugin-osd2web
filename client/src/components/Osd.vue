<template>
  <div v-show="$root.curView == 'osd'" id="osdCon">
    <div class="row dataarea">
      <div class="container menutitle" @click="$root.sendKey('Back')">
        <h3 class="">
          <icon v-if="!$root.isOnlyView" name="osd-back"></icon>
          {{ title }}
        </h3>
      </div>
      <div class="eventarea col-12 col-md-9">
        <o2w-textmenu></o2w-textmenu>
        <o2w-event :event="event"></o2w-event>
        <o2w-textarea></o2w-textarea>
        <o2w-osdbuttons />
      </div>
      <div class="rightarea pr-2 col-12 col-md-3">
        <div class="timerarea">
          <o2w-timer />
        </div>
        <o2w-customdata class="customarea" />
        <o2w-recording class="recordingarea" />
      </div>
      <o2w-statusbar />
    </div>
  </div>
</template>

<script>

var common=require("common");
common.Icon.register({"osd-back":{"width":1280,"height":1792,"paths":[{"d":"M1171 301l-531 531 531 531q19 19 19 45t-19 45l-166 166q-19 19-45 19t-45-19l-742-742q-19-19-19-45t19-45l742-742q19-19 45-19t45 19l166 166q19 19 19 45t-19 45z"}]}})

var maxLines= 0;

function getClearData(){
  return {
      title: '',
      category: -1,
      event: {},
      maxLines: maxLines                                       // maximale Anzahl Zeilen, die der Client darstellen kann
  }
}
export default {
    name: 'o2wOsd',
    data: function() {
        return getClearData();
    },
    created() {
        let menuItem= {
            label: '_O_SD',
            isHidden: function(navComp){
                return !navComp.$root.isActive
            },
            key: 'menu'
        };
        this.$root.$data.menuItems.push(menuItem);
        this.$root.$on("clearmenu", (data) => {
            let clearData= getClearData();
            for (let key in clearData)
                this[key]= clearData[key];
            this.$root.$set(menuItem, "on",false);
        });
        this.$root.$on("menu", (data) => {
            this.category = data.category;
            this.title = data.title;
            this.$root.$set(menuItem, "on",true);
        });
        this.$root.$on("event", (data) => {
            this.event = data;
        });
        this.$root.$on("rolechange", (data) => {
            this.sendMaxLines(null, this.$root.isOnlyView ? null : 50);
        });

        if (this.$root.isOnlyView){
            let lastResize= 0;
            let checkInt= false;
            let _this= this;
            function checkResize(ev){
                if (!checkInt){
                    checkInt= window.setInterval(checkResize,500);
                } else{
                    if ( (new Date().getTime() - lastResize) > 500 ){
                        _this.sendMaxLines(null);
                        checkInt= !!window.clearInterval(checkInt);
                    }
                }
                lastResize= new Date().getTime();
            }
            window.addEventListener('resize', checkResize);
        }

        //window.addEventListener('resize', this.checkButtonHeight);
    },
    updated() {
        if (this.title){
             this.$root.$emit("curView", "osd");
             if (this.$root.isOnlyView)
                window.addEventListener('resize', this.sendMaxLines);
        } else {
             this.$root.$emit("curView", null);
             window.removeEventListener('resize', this.sendMaxLines);
        }
        //this.checkButtonHeight();
    },
    methods: {
        sendMaxLines(ev, linesMax) { //  header - buttons
            let max = linesMax || common.maxLinesCalc.getMax();
            if (max != maxLines) {
                maxLines = max;
                let data = [];
                for (let i = 0; i < eMenuCategory.length; i++) data.push({
                    "category": i,
                    "maxlines": max,
                    "shape": eMenuCategory[i].shape
                });
                this.$root.$emit("send", {
                    "event": "maxlines",
                    object: {
                        "categories": data
                    }
                });
            }
        }/*,
        checkButtonHeight() {
            let buttons = document.getElementById('buttons');
            if (buttons)
                buttons.parentNode.style.paddingBottom = buttons.offsetHeight + 'px';
        }*/
    }
}

const eMenuCategory = [
    {
        "category": 'mcUnknown',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcMain',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSchedule',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcScheduleNow',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcScheduleNext',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcChannel',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcChannelEdit',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcTimer',
        "maxlines": 100,
        "shape": 1 //4 für ausführliche Informationen
    }, {
        "category": 'mcTimerEdit',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcRecording',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcRecordingInfo',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcRecordingEdit',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcPlugin',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcPluginSetup',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetup',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupOsd',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupEpg',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupDvb',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupLnb',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupCam',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupRecord',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupReplay',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupMisc',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcSetupPlugins',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcCommand',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcEvent',
        "maxlines": 100,
        "shape": 1 //4 für ausführliche Informationen
    }, {
        "category": 'mcText',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcFolder',
        "maxlines": 100,
        "shape": 1
    }, {
        "category": 'mcCam',
        "maxlines": 100,
        "shape": 1
    }
];
eMenuCategory['-1'] = {
    "category": 'mcUndefined',
    "maxlines": 100,
    "shape": 1
};

</script>
