<script lang="ts">
    export let label: string;
    export let value: number;
    export let min = 0;
    export let max = 16;
    export let step = 1;
    export let onChange = () => {};
    export let disabled = false;
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<div class="flex-row flex items-center">
    <input 
        type="number"
        {min} 
        {max}
        {step}
        id={label}
        {disabled}
        class="input input-bordered h-8 w-20"
        bind:value={value}
        on:change={onChange}
    />
</div>